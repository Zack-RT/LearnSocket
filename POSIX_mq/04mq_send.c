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
    if (argc < 2){
        fprintf(stderr, "Usage: %s <prio>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    mqd_t mqid = mq_open("/abc", O_WRONLY);
    if(mqid == (mqd_t)-1){
        ERR_EXIT("mq_open");
    }
    stu s;
    strcpy(s.name, "test");
    s.age = 20;
    unsigned prio = atoi(argv[1]);
    int ret = mq_send(mqid, (const char *)&s, sizeof(s), prio);
    if(ret < 0){
        ERR_EXIT("mq_open");
    }
    mq_close(mqid);
    return 0;   
}