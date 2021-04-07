#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "specifiedLen.h"
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/select.h>

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
    int clients[FD_SETSIZE]; // 定义一个数组，用于保存连接套接字的描述符
    for (size_t i = 0; i < FD_SETSIZE; i++)
    {
        clients[i] = -1;
    }
    
    int nready; // 用来记录select查询得到的结果：准备就绪的文件描述符个数
    int maxfd = listenfd; // 刚开始描述符的最大值就是监听套接字
    fd_set rset; // 准备就绪的套接字集合
    fd_set allset; // 所有套接字的集合
    FD_ZERO(&rset); //集合置空
    FD_ZERO(&allset); //集合置空
    FD_SET(listenfd, &allset); // 将监听套接口放入allset中
    while (1)
    {
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL); // 只关心可读事件
        if(nready == -1){
            if(errno == EINTR) continue; // 如果是系统中断导致报错，那就忽略
            ERR_EXIT("select");
        }
        if(nready == 0) continue; // 没有设置超时，这个条件不会发生

        if(FD_ISSET(listenfd, &rset)){ // 监听套接口产生事件
            peerlen = sizeof(peeraddr);
            conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen); // 此时调用accept不会阻塞
            if(conn < 0) ERR_EXIT("accept");
            int i;
            for(i = 0; i < FD_SETSIZE; i++){
                if(clients[i] < 0){// 找个空闲的位置将其保存
                    clients[i] = conn;
                    break;
                }
            }
            if(i == FD_SETSIZE){
                ERR_EXIT("too many clients");
            }
            printf("ip=%s, port=%d\n", inet_ntoa(peeraddr.sin_addr), peeraddr.sin_port);
            FD_SET(conn, &allset);
            if(conn > maxfd) // 每次添加新的描述符时，要检测更新最大的描述符
                maxfd = conn;
            if(--nready <= 0) // 事件都处理完了
                continue;
        }

        for(int i = 0; i < FD_SETSIZE; i++){
            conn = clients[i];
            if(conn == -1)
                continue;
            if(FD_ISSET(conn, &rset)){//已连接套接口有事件
                char recvbuf[1024];
                memset(recvbuf, 0, sizeof(recvbuf));//接收包头
                int readSize = readline(conn, recvbuf, sizeof(recvbuf)); // 先接收包头
                if(readSize == -1){
                   ERR_EXIT("readline");
                }
                else if(readSize == 0){ // 对等方关闭
                    printf("client close\n");
                    FD_CLR(conn, &allset);
                    clients[i] = -1;
                    close(conn);
                }
                fputs(recvbuf, stdout);
                sleep(5); // 模拟网络延迟，客户端已关闭套接字，但服务端并未读取到RST段还会继续往socket当中写入数据
                writen(conn, recvbuf, strlen(recvbuf)); 
                if(--nready <= 0){
                        break;
                }
            }
        }
    }
    return 0;
}