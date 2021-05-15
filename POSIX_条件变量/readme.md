# POSIX条件变量

## 条件变量
- 当一个线程互斥地访问某个变量时，它可能发现在其它线程改变状态之前，它什么也做不了。
- 例如一个线程访问队列时，发现队列为空，它只能等待，直到其它线程将一个节点添加到队列中。这种情况就需要用到条件变量。


## 条件变量函数条件变量使用规范
```
pthread_cond_init
pthread_cond_destroy
pthread_cond_wait
pthread_cond_signal
pthread_cond_broadcast
```

等待条件代码
```
pthread_mutex_lock(&mutex);
while(条件为假)
    pthread_cond_wait(cond, mutex);
修改条件
pthread_mutex_unlock(&mutex);
```
给条件发送信号代码
```
pthread_mutex_lock(&mutex);
设置条件为真
pthread_cond_signal(cond);pthread_mutex_unlock(&mutex);
```

- pthread_cond_wait的运行
  1. 对mutex解锁
  2. 等待条件，直到有其他线程向它发起通知
  3. 对mutex重新上锁

- pthread_cond_signal的运行
  - 向第一个等待的线程发起通知，如果没有线程处于等待状态那么通知会被丢弃

- pthread_cond_broadcast
  - 向所有等待的线程发起通知

## 使用条件变量解决生产者消费者问题
[pctest.c](pctest.c)