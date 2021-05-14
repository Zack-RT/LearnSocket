#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <pthread.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

int main()
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    int state;
    pthread_attr_getdetachstate(&attr, &state);
    if(state == PTHREAD_CREATE_JOINABLE){
        printf("detachstate: PTHREAD_CREATE_JOINABLE.\n");
    }
    if(state == PTHREAD_CREATE_DETACHED){
        printf("detachstate: PTHREAD_CREATE_DETACHED.\n");
    }

    size_t size;
    pthread_attr_getstacksize(&attr, &size);
    printf("stacksize=%lu\n", size);

    pthread_attr_getguardsize(&attr, &size);
    printf("guardsize:%lu\n", size);

    int scope;
    pthread_attr_getscope(&attr, &scope);
    if(scope == PTHREAD_SCOPE_PROCESS){
        printf("scope: PTHREAD_SCOPE_PROCESS.\n");
    }
    if(scope == PTHREAD_SCOPE_SYSTEM){
        printf("scope: PTHREAD_SCOPE_SYSTEM.\n");
    }

    int policy;
    pthread_attr_getschedpolicy(&attr, &policy);
    if(policy == SCHED_FIFO){ // 先进先出
        printf("policy: SCHED_FIFO\n");
    }
    if(policy == SCHED_RR){ // 抢占
        printf("policy: SCHED_RR\n");
    }
    if(policy == SCHED_OTHER){
        printf("policy: SCHED_OTHER\n");
    }

    int inheritsched;
    pthread_attr_getinheritsched(&attr, &inheritsched);
    if(inheritsched == PTHREAD_INHERIT_SCHED){
        printf("inherisched: PTHREAD_INHERIT_SCHED\n");
    }
    if(inheritsched == PTHREAD_EXPLICIT_SCHED){
        printf("inherisched: PTHREAD_EXPLICIT_SCHED\n");
    }

    struct sched_param param;
    pthread_attr_getschedparam(&attr, &param);
    printf("sched priority:%d\n", param.sched_priority);

    pthread_attr_destroy(&attr);
    
    int level = pthread_getconcurrency();
    printf("level:%d\n", level);

    return 0;
}