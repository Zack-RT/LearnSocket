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

void echo_cli(int msgid)
{
    int32_t pid = getpid();
    struct msgbuf msg;
    memset(&msg, 0, sizeof(msg));
    int n;
    while (fgets(msg.mtext+4, MSGMAX-4, stdin) != NULL)
    {
        msg.mtype = C2SMSG;
        *((int32_t*)msg.mtext) = pid;
        if(msgsnd(msgid, &msg, 4+strlen(msg.mtext+4), 0) < 0){
            ERR_EXIT("msgsnd");
        }
        memset(&msg, 0, sizeof(msg));
        if((n = msgrcv(msgid, &msg, MSGMAX, pid, 0)) < 0){
            ERR_EXIT("msgrcv");
        }
        fputs(msg.mtext+4, stdout);
        memset(&msg, 0, sizeof(msg));
    }
}

int main(int argc, char *argv[])
{
    
    int msgid = msgget(1234, 0);
    if(msgid < 0){
        ERR_EXIT("msgget");
    }
    echo_cli(msgid);
    
    return 0;
}
