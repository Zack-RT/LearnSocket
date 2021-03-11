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

struct packet
{
    int len; // 包头，存放包体中实际的内容长度
    char buf[1024]; // 包体
};

int main(){
    int sock; 
    if((sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        exit(-1);
    
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // ipv4协议族
    servaddr.sin_port = htons(5188); // 端口号
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");// 指定服务器的地址

    if(connect(sock, (struct sockaddr *)& servaddr, sizeof(servaddr)) < 0) // 主动状态下的套接字发起连接
        exit(-2);
    
    struct packet sendbuf;
    struct packet recvbuf;
    memset(&sendbuf, 0, sizeof(sendbuf));
    memset(&recvbuf, 0, sizeof(recvbuf));
    int n; // 包头长：主机字节序
    while(fgets(sendbuf.buf, sizeof(sendbuf.buf), stdin) != NULL){
        // 发送消息
        n = strlen(sendbuf.buf);
        sendbuf.len = htonl(n);
        writen(sock, &sendbuf, 4+n); //+4算上包头长度
        // 先接收包头
        int readSize = readn(sock, &(recvbuf.len), 4); 
        if(readSize == -1){
            ERR_EXIT("read");
        }
        else if(readSize < 4){
            printf("server close\n");
            break;
        }
        // 接收包体
        n = ntohl(recvbuf.len);
        readSize = readn(sock, recvbuf.buf, n);
        if(readSize == -1){
            ERR_EXIT("read");
        }
        else if(readSize < n){ // 不足n个字节认为对方关闭连接
            printf("server close\n");
            break;
        }
        fputs(recvbuf.buf, stdout);
        memset(&sendbuf, 0, sizeof(sendbuf));
        memset(&recvbuf, 0, sizeof(recvbuf));
    }
    close(sock);
}