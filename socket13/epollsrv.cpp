#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "socketutils.h"
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/epoll.h>
#include <vector>
#include <algorithm>

using EventList = std::vector<struct epoll_event>;

void handle_sigchld(int sig){
    //wait(NULL);
    while((waitpid(-1, NULL, WNOHANG)) > 0);
}
void handle_sigpipe(int sig){
    printf("recive a signal: %d\n", sig);
}

int main(){
    signal(SIGCHLD, handle_sigchld);
    signal(SIGPIPE, handle_sigpipe);
    int listenfd; 
    if((listenfd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");
    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // ipv4协议族
    servaddr.sin_port = htons(5188); // 端口号
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); 

    int on = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt");

    if(bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");

    if(listen(listenfd, SOMAXCONN))
        ERR_EXIT("listen");
    
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int conn;
    int i;
    int nready;
    int count = 0;
    
    std::vector<int> clients;
    int epollfd = epoll_create1(EPOLL_CLOEXEC); // 创建一个epoll实例
    // 创建一个epoll事件结构体，关联接听套接口
    struct epoll_event event;
    event.data.fd = listenfd; // epoll_event的data字段用于存储用户指定的信息
    event.events = EPOLLIN | EPOLLET; // 关注可写事件并以边缘的方式触发
    epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);
    
    EventList events(16);
    while (1)
    {
        nready = epoll_wait(epollfd, &*events.begin(), static_cast<int>(events.size()), -1);
        if(nready == -1){
            if(errno == EINTR) continue; // 如果是系统中断导致报错，那就忽略
            ERR_EXIT("select");
        }
        if(nready == 0) continue; // 没有设置超时，这个条件不会发生
        if((size_t)nready == events.size()){
            events.resize(events.size()*2);
        }
        for(i = 0; i < nready; i++){ // 逐个处理
            if(events[i].data.fd == listenfd){ // 监听套接口产生事件
                peerlen = sizeof(peeraddr);
                conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
                if(conn < 0) ERR_EXIT("accept");
                printf("ip=%s, port=%d\n", inet_ntoa(peeraddr.sin_addr), ntohs(peeraddr.sin_port));
                printf("connection number: %d\n", ++count);
                clients.push_back(conn);
                activate_nonblock(conn); // 设置为非阻塞模式
                event.data.fd = conn;
                event.events = EPOLLIN | EPOLLET;
                epoll_ctl(epollfd, EPOLL_CTL_ADD, conn, &event);
            }
            else if(events[i].events & EPOLLIN){ // 其他连接套接口产生可读事件
                conn = events[i].data.fd;
                char recvbuf[1024];
                memset(recvbuf, 0, sizeof(recvbuf));//接收包头
                int readSize = readline(conn, recvbuf, sizeof(recvbuf)); // 先接收包头
                if(readSize == -1){
                   ERR_EXIT("readline");
                }
                else if(readSize == 0){ // 对等方关闭，关闭套接口与其关联的epoll描述符
                    printf("client close\n");
                    close(conn);
                    event = events[i];
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, conn, &event);
                    clients.erase(std::find(clients.begin(),clients.end(), conn));
                }
                fputs(recvbuf, stdout);
                writen(conn, recvbuf, strlen(recvbuf)); 
            }
        }
    }
    return 0;
}