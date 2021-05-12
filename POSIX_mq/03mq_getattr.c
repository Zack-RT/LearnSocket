#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

int main(void)
{
    mqd_t mqid = mq_open("/abc", O_CREAT | O_RDWR, 0666, NULL);
    if(mqid == (mqd_t)-1){
        ERR_EXIT("mq_open");
    }
    printf("mq_open succ\n");
    // The msg_len argument specifies the size of the buffer pointed to by msg_ptr; this must be greater than or equal to the mq_msgsize attribute of the queue (see mq_getattr(3)).
    struct mq_attr ma;
    mq_getattr(mqid, &ma);
    printf("max #msg=%ld max #bytes/msg=%ld #currently on queue=%ld\n",
        ma.mq_maxmsg, ma.mq_msgsize, ma.mq_curmsgs);
    mq_close(mqid);
    return 0;   
}