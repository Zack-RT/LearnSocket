#include "threadpool.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

void *mytask(void *arg)
{
    printf("thread 0x%x is working on task %d\n", (int)pthread_self(), *(int*)arg);
    sleep(1);
    free(arg);
    return NULL;
}

int main(void)
{
    threadpool_t pool;
    threadpool_init(&pool, 3);

    for(int i = 0; i < 10; i++)
    {
        int *arg = (int *)malloc(sizeof(int));
        *arg = i;
        threadpool_add_task(&pool, mytask, arg);
    }

    sleep(10); // 等待十秒，等所有任务执行完毕再销毁
    threadpool_destory(&pool);
    return 0;
}