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
    struct msqid_ds buf;
    msgctl(msgid, IPC_STAT, &buf);
    sscanf("600", "%o", &buf.msg_perm.mode);
    msgctl(msgid, IPC_SET, &buf);
    return 0;
}