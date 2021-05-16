#include "threadpool.h"
#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

static void *thread_rountine(void *arg)
{
    struct timespec abstime;
    int timeout = 0;
    printf("thread 0x%x start.\n", (int)pthread_self());
    threadpool_t *pool = (threadpool_t *)arg;
    while (1)
    {
        condition_lock(&pool->ready);
        // 等待队列中有任务到来，或者线程池销毁通知
        pool->idle++; // 当前线程空闲
        while (pool->first == NULL && pool->quit == 0)
        {
            printf("thread 0x%x is waiting\n", (int)pthread_self());
            clock_gettime(CLOCK_REALTIME, &abstime);
            abstime.tv_sec += 2;
            int ret = condition_timedwait(&pool->ready, &abstime);
            if(ret == ETIMEDOUT){
                printf("thread 0x%x wait timed out.\n", (int)pthread_self());
                timeout = 1;
                break;
            }
            //condition_wait(&pool->ready);
        }
        pool->idle--; // 等到条件，进入工作状态
        
        if(pool->first != NULL){ // 如果是有任务分配，那么取出任务
            task_t *t = pool->first;
            pool->first = t->next;
            // 执行任务需要一定的时间，先解锁，以便其他生产者和其他消费者能够操作队列
            condition_unlock(&pool->ready);
            t->run(t->arg);
            free(t);
            condition_lock(&pool->ready);
        }
        if(pool->quit == 1 && pool->first == NULL){ // 等待到线程池销毁通知，且任务执行完毕
            pool->counter--;
            if(pool->counter == 0){
                condition_signal(&pool->ready);
            }
            condition_unlock(&pool->ready); // 跳出循环之前要解锁
            break;
        }
        if(timeout == 1 && pool->first == NULL){ // 等待超时，自动销毁
            pool->counter--;
            condition_unlock(&pool->ready);
            break;
        }
        condition_unlock(&pool->ready);
    }
    printf("thread 0x%x exit.\n", (int)pthread_self());
    return NULL;
}

// 初始化线程池
void threadpool_init(threadpool_t *pool, int threads)
{
    // 初始化各字段
    condition_init(&pool->ready);
    pool->first = NULL;
    pool->last = NULL;
    pool->counter = 0;
    pool->idle = 0;
    pool->max_threads = threads;
    pool->quit = 0;
}

// 往线程池当中添加任务
void threadpool_add_task(threadpool_t *pool, void* (*run)(void *arg), void *arg)
{
    // 生成新任务
    task_t *newtask = (task_t *)malloc(sizeof(task_t));
    newtask->run = run;
    newtask->arg = arg;
    newtask->next = NULL;

    condition_lock(&pool->ready);
    // 将任务添加到队列
    if(pool->first == NULL){
        pool->first = newtask;
    }
    else{
        pool->last->next = newtask;
    }
    pool->last = newtask;
    
    if(pool->idle > 0){ //如果有等待线程，则唤醒其中一个来执行任务
        condition_signal(&pool->ready);
    }
    else if(pool->counter < pool->max_threads){ //没有等待线程，判断当前线程数是否超过最大值,如果没有超过则创建线程
        pthread_t tid;
        pthread_create(&tid, NULL, thread_rountine, pool);
        pool->counter++;
    }
    condition_unlock(&pool->ready);
}

// 销毁线程池
void threadpool_destory(threadpool_t *pool)
{	
    if(pool->quit != 1) 
		return; 
		
	condition_lock(&pool->ready);	
	pool->quit = 1;	
	if(pool->counter > 0) {
		if(pool->idle > 0) 
			condition_broadcast(&pool->ready);
			
		//处于执行任务状态中的线程，不会收到广播 
		//线程池需要等待执行任务状态中的线程全部退出 	
		while(pool->counter > 0) 
			condition_wait(&pool->ready);	
	}
	condition_unlock(&pool->ready);
	condition_destory(&pool->ready);
}