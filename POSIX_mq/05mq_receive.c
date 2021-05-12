#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include <string.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

typedef struct{
    char name[32];
    int age;
}stu;

int main(int argc, char **argv)
{
    mqd_t mqid = mq_open("/abc", O_RDONLY);
    if(mqid == (mqd_t)-1){
        ERR_EXIT("mq_open");
    }
    struct mq_attr ma;
    mq_getattr(mqid, &ma);

    stu s;
    unsigned prio;
    int ret = mq_receive(mqid, (char *)&s, ma.mq_msgsize, &prio);
    if(ret < 0){
        ERR_EXIT("mq_receive");
    }

    printf("name=%s, age=%d, prio=%d\n", s.name, s.age, prio);
    mq_close(mqid);
    return 0;   
}