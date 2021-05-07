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

# define MSGMAX 8192U
struct msgbuf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1];    /* message data */
};

int main(int argc, char *argv[])
{
    int flag = 0, type = 0;
    while (1)
    {
        int opt = getopt(argc, argv, "nt:");
        if(opt == '?'){
            exit(EXIT_FAILURE);
        }
        if(opt == -1) break;
        switch (opt)
        {
        case 'n':
            flag |= IPC_NOWAIT;
            break;
        case 't':
            type = atoi(optarg);
            break;
        default:
            break;
        }
    }
    int msgid = msgget(1234, 0);
    if(msgid < 0){
        ERR_EXIT("msgget");
    }
    struct msgbuf *ptr = (struct msgbuf*)malloc(sizeof(long)+MSGMAX);
    ptr->mtype = type;
    int n = -1;
    if((n = msgrcv(msgid, ptr, MSGMAX, type, flag)) < 0){
        ERR_EXIT("msgrcv");
    }
    printf("read %d bytes type=%ld\n", n, ptr->mtype);
    return 0;
}