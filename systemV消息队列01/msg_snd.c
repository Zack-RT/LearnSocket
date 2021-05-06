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

struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};

int main(int argc, char *argv[])
{
    if(argc < 3){
        fprintf(stderr, "Usage: %s <bytes> <type>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int len = atoi(argv[1]);
    int type = atoi(argv[2]);
    int msgid = msgget(1234, 0);
    if(msgid < 0){
        ERR_EXIT("msgget");
    }
    struct msgbuf *ptr = (struct msgbuf*)malloc(sizeof(long)+len);
    ptr->mtype = type;
    msgsnd(msgid, ptr, len, 0);
    return 0;
}
