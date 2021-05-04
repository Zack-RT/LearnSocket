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

int main()
{
    int sockfds[2];
    if(socketpair(PF_UNIX, SOCK_STREAM, 0, sockfds) < 0){
        ERR_EXIT("socketpait");
    }
    pid_t pid = fork();
    if(pid == -1){
        ERR_EXIT("fork");
    }
    if(pid > 0){ // 父进程
        int val = 0;
        close(sockfds[1]);
        while (1)
        {
            ++val;
            printf("sending data: [%d]\n", val);
            write(sockfds[0], &val, sizeof(val));
            read(sockfds[0], &val, sizeof(val));
            printf("data recived: [%d]\n", val);
            sleep(1);
        }
    }
    if(pid == 0){ // 子进程
        int val;
        close(sockfds[0]);
        while (1)
        {
            read(sockfds[1], &val, sizeof(val));
            ++val;
            write(sockfds[1], &val, sizeof(val));
        }
    }
    return 0;
}