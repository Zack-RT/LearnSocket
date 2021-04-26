# socket编程（十三）

### 回顾select与poll
- 相同点
	- 内核要遍历所有的文件描述符，直到找到发生事件的文件描述符。
	- 随着描述符数量的增加，效率会逐渐下降。
- 不同点
	- select能打开的最大文件描述符数量受到FD_SETSIZE和操作系统进程能打开最大文件描述符数量的限制。
	- poll能打开的最大文件描述符数量仅受到操作系统进程能打开最大文件描述符数量的限制。

### epoll使用
```
#include <sys/epoll.h>
int epoll_create(int size); // 兼容老linux内核的函数，当时的epoll通过哈希表实现，需要指定最大并发数，现在这个参数已经无用，随意填写大于0的数即可。
int epoll_create1(int flags); // 新版本的Linux内核已改用红黑树管理事件
int epoll_clt(int epfd, int op, int fd, struct epoll_event *event);
int epoll_wait(int epfd, struct epoll_event *events,
                      int maxevents, int timeout);
```
> 水平触发：不断查询是否有可用的文件描述符，有的话，内核触发事件，如果数据没有处理完，内核接着触发事件(有数据就触发)
> 边缘触发：只有当I/O状态改变时，才触发事件，每次触发一次性把数据全部处理完，因为下一次处理要等I/O状态再次改变才可以(触发就全部处理完数据)

### epoll与select、poll区别
- 相比select、poll，epoll最大的好处在于它不会随着监听fd数目的增长而降低效率。
- 内核中的select与poll的实现是采用轮询来处理的，轮询的fd数目越多自然耗时越多。
- epoll的实现是基于回调的，如果fd中有期望的事件发生就会通过回调函数将其加入epoll就绪队列中，只关心“活跃”的fd，与fd数目无关。
- 内核、用户空间内存拷贝问题。select、poll采用了内存拷贝方法，而epoll采用了共享内存的方式。
- epoll不仅会告诉进程有IO事件到来，还会告诉相关的信息，这些信息是由进程填充的，因此更具这些信息进程就能直接定位到事件，而不必遍历整个fd集合。

## epoll LT/ET模式
