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
#include <pthread.h>

void echo_srv(int fd){
    char recvbuf[1024];
    while(1){
        memset(recvbuf, 0, sizeof(recvbuf));
        int readSize = readline(fd, recvbuf, sizeof(recvbuf)); // 先接收包头
        if(readSize == -1){
            ERR_EXIT("readline");
        }
        else if(readSize == 0){
            printf("client close\n");
            break;
        }

        fputs(recvbuf, stdout);
        writen(fd, recvbuf, strlen(recvbuf)); 
    }
}

void *thread_rountine(void *arg)
{
    pthread_detach(pthread_self()); // 分离当前线程，防止产生僵线程
    int conn = *(int*)arg;
    free(arg);
    echo_srv(conn);
    printf("exiting thread ... \n");
    return NULL;
}

int main(){
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
    socklen_t peerlen = sizeof(peeraddr); //一定要初始化，否则accept返回失败
    int conn = 0; 
    pthread_t tid;
    while (1)
    {
        if((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
            ERR_EXIT("conn");
        printf("ip=%s, port=%d\n", inet_ntoa(peeraddr.sin_addr), peeraddr.sin_port);
        int *pconn = (int *)malloc(sizeof(int));
        *pconn = conn;
        int ret = pthread_create(&tid, NULL, thread_rountine, pconn); 
        if(ret != 0){
            fprintf(stderr, "pthread_create:%s\n", strerror(ret));
            exit(EXIT_FAILURE); 
        }
    }

    return 0;
}