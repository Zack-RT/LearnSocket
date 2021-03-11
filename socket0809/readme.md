# socket编程（八九）

## 五种IO模型
### 阻塞IO
一旦套接口完成连接，我们可以向系统提交recv请求接收数据，这个请求是阻塞的，直到对等方数据发送到套接口接收的缓冲区中，recv再将接收到的数据由内核空间拷贝到用户空间，并返回。
### 非阻塞IO
可通过系统调用函数将对应的fd设置为非阻塞模式
```
int fcntl(int fd, int cmd, ... /* arg */ );
fcntl(fd, F_SETFL, flag|O_NONBLOCK);
```
设置之后，发起recv请求，如果缓冲区没数据直接返回-1，不阻塞。但这种方式是轮询式非阻塞IO，需要不断的循环等待消息，让CPU处于一个忙等待状态，对资源极大的浪费，不建议使用。
### IO复用（select与poll）
使用select多个文件描述符，当一个或多个检测到有数据到来时返回，再使用recv函数从内核空间读取数据
### 信号驱动IO
使用信号配合信号处理函数来实现。该方法用的不多
### 异步IO
使用aio_read，向内核态提交一个用户空间缓冲区，然后返回，当数据到来后系统自动把数据拷贝至用户空间缓冲区，并发送一个信号。这种“推”的方式相较前面的“拉”效率最高

## select
函数原型：
```
@nfds：读写异常几何中的文件描述符的最大值+1
@readfds：读集合
@writefds：写集合
@exceptfds：异常集合
@timeout：超时结构体
int select(int nfds, fd_set *readfds, fd_set *writefds,
            fd_set *exceptfds, struct timeval *timeout);

void FD_ZERO(fd_set *fdset); //将一个fd_set类型变量的所有位都设为0
void FD_CLR(int fd, fd_set *fdset);  //将fd从fdset中清除
void FD_SET(int fd, fd_set *fd_set);  //将fd添加进fdset当中
int FD_ISSET(int fd, fd_set *fdset); //判断fd是否在fdset当中
```
select相当于一个管理者，管理着多个IO，一旦其中的一个或者多个IP检测到我们所感兴趣的事件，select函数返回，返回值为检测到的事件个数，并通过指针传递出哪些IO事件。再遍历这些事件进而去处理。</br>
- 缺陷：select监听得到的事件返回后还是只能依次处理，只能实现并发处理IO而非并行处理IO，无法充分利用多核CPU。

## 套接口读、写、异常事件发生条件
- 可读
    - 套接口缓冲区有数据可读。
    - 连接的另一方关闭，即接收到FIN，读操作返回0。
    - 如果是监听套接口，已完成连接队列非空时。
    - 套接口上发生了一个错误待处理，错误可以通过getsockopt指定SO_ERROR选项来获取。
- 可写
    - 套接口发送缓冲区有空间容纳数据。
    - 连接的写一半关闭。即收到RST段之后，再次调用写操作。
    - 套接口上发生了一个错误待处理，错误可以通过getsockopt指定SO_ERROR选项来获取。
- 异常
    - 套接口存在带外数据