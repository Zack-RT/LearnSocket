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
    int msgid = msgget(1234, 0);
    if(msgid < 0){
        ERR_EXIT("msgget");
    }
    printf("msgget success, msgid=%d\n",msgid);
    int ret = msgctl(msgid, IPC_RMID, NULL);
    if(ret < 0){
        ERR_EXIT("msgctl");
    }
    printf("ipcrm -q %d\n", msgid);
    return 0;
}