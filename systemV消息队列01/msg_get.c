#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

int main(void)
{
    int msgid = msgget(1234, 0666 | IPC_CREAT);
    if(msgid < 0){
        ERR_EXIT("msgget");
    }
    printf("msgget success, msgid=%d\n", msgid);
    return 0;
}