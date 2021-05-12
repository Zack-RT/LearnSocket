#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#include <string.h>
#include <signal.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

typedef struct{
    char name[32];
    int age;
}stu;

mqd_t mqid = -1;
size_t size = 0U;
struct sigevent sigev;

void handle_sigusr1(int sig)
{
    mq_notify(mqid, &sigev); // 多次注册
    stu s;
    unsigned prio;
    ssize_t ret = mq_receive(mqid, (char*)&s, size, &prio);
    if(ret < 0){
        ERR_EXIT("mq_receive");
    }
    printf("name=%s, age=%d, prio=%u\n", s.name, s.age, prio);
}

int main(int argc, char **argv)
{
    mqid = mq_open("/abc", O_RDONLY);
    if(mqid == (mqd_t)-1){
        ERR_EXIT("mq_open");
    }

    struct mq_attr ma;
    mq_getattr(mqid, &ma);
    size = ma.mq_msgsize;
    
    signal(SIGUSR1, handle_sigusr1);
    sigev.sigev_notify = SIGEV_SIGNAL; // 用信号通知
    sigev.sigev_signo = SIGUSR1; // 指定信号类型
    mq_notify(mqid, &sigev);

    while (1)
    {
        pause();
    }
    
    mq_close(mqid);
    return 0;   
}