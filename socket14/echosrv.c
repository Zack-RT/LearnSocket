#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include "socketutils.h"

void echo_srv(int sock)
{
    char recvbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    peerlen = sizeof(peeraddr);
    int n = 0;
    while (1)
    {
        memset(recvbuf, 0, sizeof(recvbuf));
        n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*)&peeraddr, &peerlen);
        if(n == -1){
            if(errno == EINTR)
                continue;
            ERR_EXIT("recvfrom");
        }
        else if(n > 0){
            fputs(recvbuf, stdout);
            sendto(sock, recvbuf, n, 0, (struct sockaddr*)&peeraddr, peerlen);
        }
    }
    close(sock);
}

int main(void)
{
    int sock;
    if((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
        ERR_EXIT("socket");
    }
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        ERR_EXIT("bind");
    }
    echo_srv(sock);
    return 0;
}