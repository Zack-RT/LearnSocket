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
#include <sys/select.h>
#include <poll.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

void echo_srv(int fd){
    char recvbuf[1024];
    while(1){
        memset(recvbuf, 0, sizeof(recvbuf));
        //接收包头
        int readSize = readline(fd, recvbuf, sizeof(recvbuf)); // 先接收包头
        if(readSize == -1){
            ERR_EXIT("readline");
        }
        else if(readSize == 0){
            printf("client close\n");
            break;
        }

        fputs(recvbuf, stdout);
        //再回射回去
        writen(fd, recvbuf, strlen(recvbuf)); 
    }
}

void handle_sigchld(int sig){
    //wait(NULL);
    while((waitpid(-1, NULL, WNOHANG)) > 0); // 等待所有子进程退出
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

    // 绑定一个本地地址到套接字
    if(bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");

    if(listen(listenfd, SOMAXCONN))
        ERR_EXIT("listen");
    
    struct sockaddr_in peeraddr;
    socklen_t peerlen; //一定要初始化，否则accept返回失败
    int conn = 0;
    struct pollfd clients[1024]; 
    for (size_t i = 0; i < 1024; i++)
    {
        clients[i].fd = -1; // 将描述符至为-1，表示空闲
    }
    
    int nready = 0;
    int maxi = 0; // 记录当前进程的最大文件描述符
    clients[0].fd = listenfd; // 首先要关注监听套接口
    clients[0].events = POLLIN; // 只对可写事件感兴趣
    while (1)
    {
        nready = poll(clients, maxi+1, -1);
        if(nready == -1){
            if(errno == EINTR) continue; // 如果是系统中断导致报错，那就忽略
            ERR_EXIT("select");
        }
        if(nready == 0) continue; // 没有设置超时，这个条件不会发生
        if(clients[0].revents & POLLIN){ // 监听套接口产生可读事件，接受连接
            peerlen = sizeof(peeraddr);
            conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen); // 此时调用accept不会阻塞
            printf("%d connect established\n", conn);
            if(conn < 0) ERR_EXIT("accept");
            int i;
            for(i = 1; i < 1024; i++){
                if(clients[i].fd < 0){// 找个空闲的位置将其保存
                    clients[i].fd = conn;
                    if(i > maxi){
                        maxi = i;
                    }
                    break;
                }
            }
            if(i == 1024){
                ERR_EXIT("too many clients");
            }
            printf("ip=%s, port=%d\n", inet_ntoa(peeraddr.sin_addr), peeraddr.sin_port);
            clients[i].events = POLLIN;
            if(--nready <= 0) // 事件都处理完了
                continue;
        }

        for(int i = 1; i < maxi+1; i++){
            conn = clients[i].fd;
            printf("%d connect read, maxi=%d, i=%d\n", conn, maxi, i);
            if(conn == -1)
                continue;
            if(clients[i].revents & POLLIN){
                char recvbuf[1024];
                memset(recvbuf, 0, sizeof(recvbuf));//接收包头
                int readSize = readline(conn, recvbuf, sizeof(recvbuf)); // 先接收包头
                if(readSize == -1){
                   ERR_EXIT("readline");
                }
                else if(readSize == 0){ // 对等方关闭
                    printf("client close\n");
                    clients[i].fd = -1;
                    close(conn);
                }
                fputs(recvbuf, stdout);
                writen(conn, recvbuf, strlen(recvbuf)); 
                if(--nready <= 0){
                        break;
                }
            }
        }
    }
    return 0;
}