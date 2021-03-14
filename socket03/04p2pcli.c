#include <sys/socket.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>

#define ERR_EXIT(msg) \
    { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }

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
    
    pid_t pid;
    pid = fork();
    if(pid < 0){
        ERR_EXIT("fork");
    }
    else if(pid == 0){ //子进程接收数据
        char recvbuf[1024] = {0};
        while(1){
            memset(recvbuf, 0, sizeof(recvbuf));
            int ret = read(sock, recvbuf, sizeof(recvbuf));
            if(ret < 0){
                ERR_EXIT("read");
            }
            else if(ret == 1){
                printf("peer close\n");
            }
            else{
                fputs(recvbuf, stdout);
            }
        }
    }
    else{ //父进程发送数据
        char sendbuf[1024] = {0};
        while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL){
            int ret = write(sock, sendbuf, strlen(sendbuf));
            if(ret < 1){
                ERR_EXIT("write");
            }
            memset(sendbuf, 0, sizeof(sendbuf));
        }
        
    }
    return 0;
}
