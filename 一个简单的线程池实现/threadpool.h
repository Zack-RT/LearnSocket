#include "condition.h"

typedef struct task
{
    void *(*run)(void *arg); // 任务回调函数
    void *arg; // 回调函数参数
    struct task *next;
} task_t;

typedef struct threadpool
{
    condition_t ready; // 任务准备就绪或者线程池销毁通知
    task_t *first, *last; // 任务队列的头指针和尾指针
    int counter; // 线程池中当前线程数
    int idle; //线程池中当前正在等待任务的线程数
    int max_threads; // 线程池中最大允许的线程数
    int quit; // 销毁线程池的时候设置为1
} threadpool_t;

// 初始化线程池
void threadpool_init(threadpool_t *pool, int threads);
// 往线程池当中添加任务
void threadpool_add_task(threadpool_t *pool, void* (*run)(void *arg), void *arg);
// 销毁线程池
void threadpool_destory(threadpool_t *pool);