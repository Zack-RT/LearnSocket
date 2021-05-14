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

pthread_key_t key_tsd;
pthread_once_t once_control = PTHREAD_ONCE_INIT;

typedef struct{
    pthread_t tid;
    char *str;
}tsd_t;

void destroy_rountine(void *value)
{
    printf("destory .. \n");
    free(value);
}

void once_rountine(void)
{
    pthread_key_create(&key_tsd, destroy_rountine);
    printf("key init ...\n");
}

void* thread_rountine(void *arg)
{
    pthread_once(&once_control, once_rountine); // 只有第一个线程进来会调用
    tsd_t *value = (tsd_t*)malloc(sizeof(tsd_t));
    value->tid = pthread_self();
    value->str = (char*)arg;
    pthread_setspecific(key_tsd, value);

    printf("%s setspecific %p\n", (char*)arg, value);
    value = pthread_getspecific(key_tsd);
    printf("tid=0x%x, str=%s\n", (unsigned)value->tid, value->str);
    sleep(2);
    value = pthread_getspecific(key_tsd);
    printf("tid=0x%x, str=%s\n", (unsigned)value->tid, value->str);
    return NULL;
}
int main()
{
    //pthread_key_create(&key_tsd, destroy_rountine); // 创建的key是每个线程都有的，但不同线程的key所指向的数据不同
    pthread_t tid1, tid2;
    pthread_create(&tid1, NULL, thread_rountine, "thread1");
    pthread_create(&tid2, NULL, thread_rountine, "thread2");

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    pthread_key_delete(key_tsd);
    return 0;
}