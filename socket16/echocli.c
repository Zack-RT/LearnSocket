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

void echo_cli(int sock);

int main()
{
    int sock;
    if((sock = socket(PF_UNIX, SOCK_STREAM, 0)) < 0){
        ERR_EXIT("socket");
    }
    struct sockaddr_un servaddr;
    memset(&servaddr, 0, sizeof(struct sockaddr_un));
    servaddr.sun_family = AF_UNIX;
    strcpy(servaddr.sun_path, "/tmp/test_socket");
    if(connect(sock, (struct sockaddr*)&servaddr, sizeof(struct sockaddr)) < 0){
        ERR_EXIT("connet");
    }
    echo_cli(sock);
    return 0;
}

void echo_cli(int sock)
{
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        write(sock, sendbuf, strlen(sendbuf));
        read(sock, recvbuf, sizeof(recvbuf));
        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }
    close(sock);
}