# POSIX信号量与互斥锁

## POSIX信号量相关函数
```
sem_open
sem_close
sem_unlink

sem_init
sem_destory

sem_wait
sem_post
```

## POSIX互斥锁相关函数
```
pthread_mutex_init
pthread_mutex_lock
pthread_mutex_unlock
pthread_mutex_destory
```

## 生产者消费者问题
[问题回顾](../systemVshm&sem/readme.md)

## 自旋锁与读写锁介绍

### 自旋锁
- 自旋锁类似于互斥锁，它的性能比互斥锁更高。
- 自旋锁与互斥锁很重要的一个区别在于，线程在申请自旋锁的时候，线程不会被挂起，它处于忙等待的状态。
```
pthread_spin_init
pthread_spin_destroy
pthread_spin_lock
pthread_spin_unlock
```

### 读写锁
- 只要没有线程持有给定的读写锁用于写，那么任意数目的线程可以持有读写锁用于读
- 仅当没有线程持有某个给定的读写锁用于读或用于写时，才能分配读写锁用于写
- 读写锁用于读称为共享锁，读写锁用于写称为排它锁
```
pthread_rwlock_init
pthread_rwlock_destroy
int pthread_rwlock_rdlock
int pthread_rwlock_wrlockint pthread_rwlock_unlock
```