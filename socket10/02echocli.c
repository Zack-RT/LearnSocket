#include <sys/socket.h>
#include <sys/types.h>
#include <sys/select.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "specifiedLen.h"

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

void echo_cli(int sock){
    fd_set rset;
    FD_ZERO(&rset);
    int nready;
    int fd_stdin = fileno(stdin);
    int maxfd = (fd_stdin > sock ? fd_stdin : sock);
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    int stdinEOF = 0;
    while(1){
        if(stdinEOF == 0){
            FD_SET(fd_stdin, &rset);
        }
        FD_SET(sock, &rset);
        nready = select(maxfd+1, &rset, NULL, NULL, NULL);
        if(nready == -1)
            ERR_EXIT("select");
        if(nready == 0)
            continue;
        if(FD_ISSET(sock, &rset)){
            int readSize = readline(sock, recvbuf, sizeof(sendbuf)); 
            if(readSize == -1){
                ERR_EXIT("readline");
            }
            else if(readSize == 0){
                printf("server close\n");
                break;
            }
            fputs(recvbuf, stdout);
            memset(recvbuf, 0, sizeof(recvbuf));
        }
        if(FD_ISSET(fd_stdin, &rset)){
            memset(sendbuf, 0, sizeof(sendbuf));
            if(fgets(sendbuf, sizeof(sendbuf), stdin) == NULL){
                stdinEOF = 1;
                shutdown(sock, SHUT_WR);
            }
            else{
                writen(sock, sendbuf, strlen(sendbuf)); 
                memset(sendbuf, 0, sizeof(recvbuf));
            }
        }
    }
}

int main(){
    int sock; // 与服务器创建多个连接，测试服务器处理僵进程的情况
    
    if((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        ERR_EXIT("soket");
        
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // ipv4协议族
    servaddr.sin_port = htons(5188); // 端口号
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");// 指定服务器的地址

    if(connect(sock, (struct sockaddr *)& servaddr, sizeof(servaddr)) < 0) // 主动状态下的套接字发起连接
        ERR_EXIT("connect");

    struct sockaddr_in localaddr;
    socklen_t addrlen = sizeof(localaddr);
    if(getsockname(sock, (struct sockaddr*)&localaddr, &addrlen) < 0){
        ERR_EXIT("getsockname");
    }
    printf("ip=%s, port=%d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
    echo_cli(sock);//选择一个用来通信就好
    return 0;
}