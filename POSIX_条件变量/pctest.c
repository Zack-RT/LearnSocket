#define _GNU_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include <pthread.h>
#include <semaphore.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

#define CONSUMERS_COUNT 4U
#define PRODUCERS_COUNT 2U

pthread_mutex_t g_mutex;
pthread_cond_t g_cond;

pthread_t g_thread[CONSUMERS_COUNT+PRODUCERS_COUNT];

int nready = 0;// 当前缓冲区产品的个数

void *consume(void *arg)
{
    int num = (int)arg;
    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        while (nready == 0) // pthread_cond_wait有可能虚假唤醒，所以要用while多次判断，而不能用if
        {
            printf("%d begin wait condition\n", num);
            pthread_cond_wait(&g_cond, &g_mutex); // 等待时会解锁，让其他线程进入临界区修改nready
        }
        printf("%d end wait a condition\n", num);
        --nready; // 消费产品
        pthread_mutex_unlock(&g_mutex);
        sleep(1);
    }
    return NULL;
}

void *produce(void *arg)
{
    int num = (int)arg;
    while (1)
    {
        pthread_mutex_lock(&g_mutex);
        ++nready; // 生产产品
        pthread_cond_signal(&g_cond); // 通知在等待的线程
        printf("%d signal ...\n", num);
        pthread_mutex_unlock(&g_mutex);
        sleep(2);
    }
    return NULL;
}

int main()
{
    pthread_mutex_init(&g_mutex, NULL);
    pthread_cond_init(&g_cond, NULL);
    
    for(int i = 0; i < CONSUMERS_COUNT; i++){
        pthread_create(g_thread+i, NULL, consume, (void*)i);
    }
    for(int i = 0; i < PRODUCERS_COUNT; i++){
        pthread_create(g_thread+CONSUMERS_COUNT+i, NULL, produce, (void*)i);
    }
    for(int i = 0; i < PRODUCERS_COUNT+CONSUMERS_COUNT; i++){
        pthread_join(g_thread[i], NULL);
    }

    pthread_mutex_destroy(&g_mutex);
    pthread_cond_destroy(&g_cond);

    return 0;
}