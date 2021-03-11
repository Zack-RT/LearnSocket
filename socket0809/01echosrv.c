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

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)
    
// struct packet
// {
//     int len; // 包头，存放包体中实际的内容长度, 网络字节序
//     char buf[1024]; // 包体
// };

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

int main(){
    signal(SIGCHLD, handle_sigchld);
    int listenfd; 
    // 创建一个套接字用于通信
    // 失败返回-1，成功返回一非负整数——套接口描述字，简称套接字，与文件描述符类似
    // IPPROTO_TCP也可填0 自动选择协议。SOCK_STREAM也只能通过TCP实现
    if((listenfd=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("socket");
    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // ipv4协议族
    servaddr.sin_port = htons(5188); // 端口号
    // 有三种常用的指定地址方式，任选其一：
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // 任意本机地址
    //servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");// 显示指定
    //inet_aton("127.0.0.1", &servaddr.sin_addr);

    int on = 1;
    if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
        ERR_EXIT("setsockopt");

    // 绑定一个本地地址到套接字
    if(bind(listenfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind");
    
    // 将套接字转换为监听状态，才能接受连接
    // 使用此函数后套接字变为被动，否则默认均为主动套接字
    // 主动套接字：通过connnet函数来发起连接
    // 被动套接字：通过accept来接受连接
    // 一般在socket和bind之后、accept之前调用
    if(listen(listenfd, SOMAXCONN))
        ERR_EXIT("listen");
    
    struct sockaddr_in peeraddr;
    socklen_t peerlen; //一定要初始化，否则accept返回失败
    int conn = 0;
    int clients[FD_SETSIZE];
    for (size_t i = 0; i < FD_SETSIZE; i++)
    {
        clients[i] = -1;
    }
    
    int nready;
    int maxfd = listenfd;
    fd_set rset;
    fd_set allset;
    FD_ZERO(&rset);
    FD_ZERO(&allset);
    FD_SET(listenfd, &allset);
    while (1)
    {
        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if(nready == -1){
            if(errno == EINTR) continue;
            ERR_EXIT("select");
        }
        if(nready == 0) continue;
        if(FD_ISSET(listenfd, &rset)){ // 监听套接口产生事件
            peerlen = sizeof(peeraddr);
            conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen);
            if(conn < 0) ERR_EXIT("accept");
            int i;
            for(i = 0; i < FD_SETSIZE; i++){
                if(clients[i] < 0){ 
                    clients[i] = conn;
                    break;
                }
            }
            if(i == FD_SETSIZE){
                ERR_EXIT("too many clients");
            }
            printf("ip=%s, port=%d\n", inet_ntoa(peeraddr.sin_addr), peeraddr.sin_port);
            FD_SET(conn, &allset);
            if(conn > maxfd)
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
                else if(readSize == 0){
                    printf("client close\n");
                    FD_CLR(conn, &allset);
                }
                fputs(recvbuf, stdout);
                writen(conn, recvbuf, strlen(recvbuf)); 
                if(--nready <= 0)
                        break;
            }
        }

    }
    return 0;
}