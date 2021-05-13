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

void *thread_routine(void *arg)
{
    for(int i = 0; i < 20; i++){
        printf("B");
        fflush(stdout);
        usleep(20);
        // if(i == 3){
        //     pthread_exit("ABC");
        // }
    }
    return "DEF";
}

int main(void)
{
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, thread_routine, NULL);
    if(ret != 0){
        fprintf(stderr, "pthread_create:%s\n", strerror(ret));
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < 20; i++){
        printf("A");
        fflush(stdout);
        usleep(20);
    }
    void *retvalue;
    ret = pthread_join(tid, &retvalue); // 防止主线程结束后创建的线程还未被调度
    if(ret != 0){
        fprintf(stderr, "pthread_join:%s\n", strerror(ret));
        exit(EXIT_FAILURE); 
    }
    printf("\n");
    printf("return msg=%s\n",(char*)retvalue);
    return 0;
}