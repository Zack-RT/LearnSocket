#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "specifiedLen.h"

#define ERR_EXIT(msg) \
    { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }

// struct packet
// {
//     int len; // 包头，存放包体中实际的内容长度
//     char buf[1024]; // 包体
// };

void echo_cli(int sock){
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL){
        // 发送消息
        writen(sock, sendbuf, strlen(sendbuf)); 

        int readSize = readline(sock, recvbuf, sizeof(sendbuf)); 
        if(readSize == -1){
            ERR_EXIT("readline");
        }
        else if(readSize == 0){
            printf("server close\n");
            break;
        }
        
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    close(sock);
}

int main(){
    int sock[5]; // 与服务器创建多个连接，测试服务器处理僵进程的情况
    int i;
    for(i = 0; i < 5; i++){
        if((sock[i]=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
            ERR_EXIT("soket");
        
        struct sockaddr_in servaddr;
        memset(&servaddr, 0, sizeof(servaddr));
        servaddr.sin_family = AF_INET; // ipv4协议族
        servaddr.sin_port = htons(5188); // 端口号
        servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");// 指定服务器的地址

        if(connect(sock[i], (struct sockaddr *)& servaddr, sizeof(servaddr)) < 0) // 主动状态下的套接字发起连接
            ERR_EXIT("connect");

        struct sockaddr_in localaddr;
        socklen_t addrlen = sizeof(localaddr);
        if(getsockname(sock[i], (struct sockaddr*)&localaddr, &addrlen) < 0){
            ERR_EXIT("getsockname");
        }
        printf("ip=%s, port=%d\n", inet_ntoa(localaddr.sin_addr), ntohs(localaddr.sin_port));
    } 
    echo_cli(sock[0]);//选择一个用来通信就好
    return 0;
}