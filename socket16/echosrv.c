#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/un.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)


void echo_srv(int sock);

int main(void)
{
    int listenfd;
    if((listenfd = socket(PF_UNIX, SOCK_STREAM, 0)) < 0){
        ERR_EXIT("socket");
    }
    struct sockaddr_un servaddr;
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, "/tmp/test_socket");
    unlink("/tmp/test_socket"); // 不同与TCP设置重复使用选项，UNIX可以删除文件，这样就不会提示地址重复使用了
    if(bind(listenfd, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0){ // 绑定时会自动生成对应文件
        ERR_EXIT("bind");
    }
    if(listen(listenfd, SOMAXCONN) < 0){
        ERR_EXIT("listen");
    }

    int conn;
    pid_t pid;
    while (1)
    {
        conn = accept(listenfd, NULL, NULL);
        if(conn < 0){
            if(errno == EINTR) continue;
            ERR_EXIT("accept");
        }
        pid = fork();
        if(pid == -1){
            ERR_EXIT("fork");
        }
        if(pid == 0){ // 子进程
            close(listenfd);
            echo_srv(conn);
            exit(EXIT_SUCCESS);
        }
        close(conn);
    }
    return 0;
}

void echo_srv(int sock)
{
    char recvbuf[1024];
    int n_read = 0;
    while (1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        n_read = read(sock, recvbuf, sizeof(recvbuf));
        if(n_read < 0){
            if(errno == EINTR) continue;
            ERR_EXIT("read");
        }
        else if(n_read == 0){
            printf("client close\n");
            break;
        }
        else{
            fputs(recvbuf, stdout);
            write(sock, recvbuf, strlen(recvbuf));
        }
    }
    close(sock);
}