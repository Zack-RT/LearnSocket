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
    

void do_service(int fd){
    char recvbuf[1024];
    while(1){
        memset(recvbuf, 0, sizeof(recvbuf));
        int readSize = read(fd, recvbuf, sizeof(recvbuf));
        if(readSize == 0){
            printf("client close\n");
            break;
        }
        fputs(recvbuf, stdout);
        write(fd, recvbuf, readSize);
    }
}

int main(){
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
    socklen_t peerlen = sizeof(peeraddr); //一定要初始化，否则accept返回失败
    int conn = 0; 
    pid_t pid;
    while (1)
    {
        // accept会返回一个新的已连接非监听状态的套接字
        if((conn = accept(listenfd, (struct sockaddr*)&peeraddr, &peerlen)) < 0)
            ERR_EXIT("conn");
        printf("ip=%s, port=%d\n", inet_ntoa(peeraddr.sin_addr), peeraddr.sin_port);
        pid = fork();
        if(pid == -1){
            ERR_EXIT("fork");
        }
        if(pid == 0){ // 子进程与客户端建立连接
            close(listenfd);
            do_service(conn);
            exit(EXIT_SUCCESS);
        }
        else{ // 父进程继续监听
            close(conn);
        }
    }

    return 0;
}