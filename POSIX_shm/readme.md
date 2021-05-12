# POSIX共享内存

## 相关函数
### shm_open
- 功能：创建或打开一个共享内存对象
- 原型
```
int shm_open(const char *name, int oflag, mode_t mode);
```
- 参数
  - name：共享内存的名字，命名规则同消息队列
    - 必须以/开头，并且后续不能有其他的/
    - 长度不能超过NAME_MAX
  - oflag：与open函数类似，可以是O_RDONLY, O_WRONLY, O_RDWR还可以或上O_CREATE, O_EXCL, O_NONBLOCK等组合。
  - mode：此参数总是需要设置，如果oflag没有指定O_CREATE，可指定为0。
- 返回值：成功返回非负整数文件描述符，失败返回-1

### ftruncate
- 功能：修改共享内存对象大小
- 原型
```
int ftruncate(int fd, off_t length);
```
- 参数
  - fd：文件描述符
  - length：长度
- 返回值：成功返回0，失败返回-1

### fstat
- 功能：获取共享内存对象信息
- 原型
```
int fstat(int fd, struct stat *statbuf);
```
- 参数
  - fd：文件描述符
  - statbuf：返回共享内存状态信息
- 返回值：成功返回0，失败返回-1

### shm_unlink
- 功能：删除一个共享内存对象
- 原型
  - int shm_unlink(const char *name);
- 参数
  - name：共享内存对象的名字
- 返回值：成功返回0，失败返回-1


## mmap函数
- 功能：将文件或者设备空间映射到共享内存区。
- 原型：
```
#include <sys/mman.h>
void *mmap(void *addr, size_t length, int prot, int flags,
            int fd, off_t offset);
```
- 参数
  - addr:要映射的起始地址，通常指定为NULL，让内核自动选择
  - len:映射到进程地址空间的字节数
  - prot:映射区保护方式
  - flags:标志
  - fd:文件描述符，可设置为-1，映射匿名内存空间，用于亲缘进程间通信
  - offset:从文件头开始的偏移量
- 返回值：成功返回映射到的内存区的起始地址，失败返回-1

## 示例代码
- [01shm_open](01shm_open.c)
- [02shm_unlink](02shm_unlink.c)
- [03shm_write](03shm_write.c)
- [04shm_read](04shm_read.c)