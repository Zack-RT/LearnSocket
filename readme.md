# tcp/ip socket 高级编程课程笔记与代码

## 简介

[这门课程](https://www.bilibili.com/video/BV12E41197zn)虽然视频画质音质一般，但隔着屏幕就能感受到老师的功力深厚。vim熟练使用，man查阅文档快速编写代码；gcc报错结合gdb快速定位bug；先明确需求功能与再实现API的测试驱动开发思想。理论结合实操，全程看下来收获良多。

## 一些知识点的整理

- 主机字节序、网络字节序以及大端小端的判断方式。
- 回射客户服务器：
  - 单进程一对一
  - 多进程实现并发
  - 解决粘包问题
  - IO复用实现并发：select/poll/epoll
  - POSIX多线程实现并发
- 进程间通信与常用的IPC
  - 共享内存
  - 信号量
  - 消息队列
  - 条件变量

--- 

## 几个小综合

1. [基于UDP实现的P2P聊天室](./socket15/)
2. [基于共享内存与信号量实现FIFO队列](./systemVshm&sem/)
3. [线程池：生产者消费者问题的实际应用](./一个简单的线程池实现/)

## 目录

前5P为TCP/IP理论部分，没有做笔记与代码，socket编程部分文件夹名与分p对应。

- [p23-p24 进程间通信介绍](./IPC/)
- [p25-p27 systemV消息队列](./systemV消息队列01/)
- [p28 共享内存介绍](./共享内存/)
- [p29 systemV共享内存](./systemV共享内存/)
- [p30-p32 systemV信号量](./systemVsemaphore/)
- [p33 systemV共享内存与信号量综合](./systemVshm&sem/)
- [p34 POSIX消息队列](./POSIX_mq/)
- [p35 POSIX共享内存](./POSIX_shm/)
- [p36 线程介绍](./POSIX_multi-thread/线程介绍.md)
- [p37-p38 POSIX线程](./POSIX_multi-thread/)
- [p39 POSIX信号量与互斥锁](./POSIX_sem/)
- [p40 POSIX条件变量](./POSIX_条件变量/)
- [p41 一个简单的线程池实现](./一个简单的线程池实现/)