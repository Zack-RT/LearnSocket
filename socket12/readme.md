# socket编程（十二）

## select的限制
- 用select实现的并发服务器能达到的并发数受到两方面的限制
  1. 一个进程能打开的最大文件描述符，这可以通过调整内核参数修改。
  2. select中的fd_set集合容量的限制（FD_SETSIZE）,这需要重新编译内核。
> ulimit -n 命令查询一个进程能打开的最大文件描述符, 后面跟数字则能修改。还可以在程序中通过系统API来修改：
```man 2 GETRLIMIT
#include <sys/time.h>
#include <sys/resource.h>
int getrlimit(int resource, struct rlimit *rlim); //获取进程的各类资源数
int setrlimit(int resource, const struct rlimit *rlim); //设置进程的各类资源数
```

## poll
```man 2 poll
#include <poll.h>
int poll(struct pollfd *fds, nfds_t nfds, int timeout);
```
- poll没有了fd_set集合容量的限制
- 但还是收到最大文件描述符限制，不过这很好修改。但最终还是收到系统所能打开的最大文件描述符限制，这取决于系统的内存，这通常很大不会成为瓶颈。
> cat /proc/sys/fs/file-max 查看系统所能打开的最大文件描述符