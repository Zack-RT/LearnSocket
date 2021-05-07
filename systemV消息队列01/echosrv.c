#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)
# define MSGMAX 8192U
# define C2SMSG 1

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[MSGMAX];    /* message data */
};

void echo_srv(int msgid)
{
    struct msgbuf msg;
    memset(&msg, 0, sizeof(msg));
    while (1)
    {
        int n = -1;
        if((n = msgrcv(msgid, &msg, MSGMAX, C2SMSG, 0)) < 0){
            ERR_EXIT("msgrcv");
        }
        int32_t pid = *((int32_t*)msg.mtext);
        msg.mtype = pid;
        fputs(msg.mtext+4, stdout);
        msgsnd(msgid, &msg, n, 0);
    }
}

int main(int argc, char *argv[])
{
    int msgid = msgget(1234, 0666 | IPC_CREAT);
    if(msgid < 0){
        ERR_EXIT("msgget");
    }
    echo_srv(msgid);
    return 0;
}