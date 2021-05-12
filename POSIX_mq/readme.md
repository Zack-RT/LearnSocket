# POSIX消息队列

## POSIX消息队列相关函数
### mq_open
- 功能：用来创建和访问一个消息队列
- 原型
```
mqd_t mq_open(const char *name, int oflag);
mqd_t mq_open(const char *name, int oflag, mode_t mode,
                struct mq_attr *attr);
```
- 参数
  - name：某个消息队列的名字
    - 必须以/开头，并且后续不能有其他的/
    - 长度不能超过NAME_MAX
  - oflag：与open函数类似，可以是O_RDONLY, O_WRONLY, O_RDWR还可以或上O_CREATE, O_EXCL, O_NONBLOCK等组合。
  - mode：如果oflag设置了O_CREATE，则需要指定mode
- 返回值：成功返回消息队列文件描述符，失败返回-1

## mq_close
- 功能：用来关闭（不等于删除）一个消息队列
- 原型
```
int mq_close(mqd_t mqdes);
```
- 参数
  - mqdes：消息队列描述符
- 返回值：成功返回0，失败返回-1

## mq_unlink
- 功能：删除消息队列（引用数-1）
- 原型
```
int mq_unlink(const char *name);
```
- 参数
  - name：消息队列的名字
- 返回值：成功返回0，失败返回-1

### mq_getattr / mq_setattr
- 功能：获取、设置消息队列属性
- 原型
```
int mq_getattr(mqd_t mqdes, struct mq_attr *attr);
int mq_setattr(mqd_t mqdes, const struct mq_attr *newattr,
                struct mq_attr *oldattr);
```
- 参数
  - mqdes：消息队列描述符
  - attr：新的属性 or 原来的属性
- 返回值：成功返回0，失败返回-1

### mq_send
- 功能：往指定的消息队列当中发送消息
- 原型
```
int mq_send(mqd_t mqdes, const char *msg_ptr,
                size_t msg_len, unsigned int msg_prio);
```
- 参数
  - mqdes：消息队列描述符
  - msg_ptr：指向消息的指针
  - msg_len：消息长度
  - msg_prio：消息优先级
- 返回值：成功返回0，失败返回-1

### mq_receive
- 功能：接受消息
- 原型
```
ssize_t mq_receive(mqd_t mqdes, char *msg_ptr,
                  size_t msg_len, unsigned int *msg_prio);
```
- 参数
  - mqdes：消息队列描述符
  - msg_ptr：指向消息的指针
  - msg_len：消息长度
  - msg_prio：返回接收到的消息优先级
- 返回值：成功返回接受到的字节数，失败返回-1
- 注意：返回指定消息队列中最高优先级的最早消息

### mq_notify
- 功能：建立或者删除消息到达通知事件
- 原型
```
int mq_notify(mqd_t mqdes, const struct sigevent *sevp);

union sigval {          /* Data passed with notification */
    int     sival_int;         /* Integer value */
    void   *sival_ptr;         /* Pointer value */
};

struct sigevent {
    int          sigev_notify; /* Notification method */
    int          sigev_signo;  /* Notification signal */
    union sigval sigev_value;  /* Data passed with
                                  notification */
    void       (*sigev_notify_function) (union sigval);
                    /* Function used for thread
                        notification (SIGEV_THREAD) */
    void        *sigev_notify_attributes;
                    /* Attributes for notification thread
                        (SIGEV_THREAD) */
    pid_t        sigev_notify_thread_id;
                    /* ID of thread to signal (SIGEV_THREAD_ID) */
};
```
- 参数
  - mqdes：消息队列描述符
  - sevp：非空表示当前消息到达且消息队列先前为空，那么将得到通知；为NULL表示撤销已注册的通知
- 返回值：成功返回0，失败返回-1
- 通知方式
  - 产生一个信号，创建一个线程执行一个指定函数
  - 通过设置sigev_notify来选择
- **注意点**
  - 任何时刻只能有一个进程可以被注册为接收某个给定队列的通知
  - 当有一个消息到达某个先前为空的队列，而且已有一个进程被注册为接收该队列的通知时，只有没有任何线程阳塞在该队列的mq_receive调用的前提下，通知才会发出。
  - 当通知被发送给它的注册进程时，其注册被撤消。进程必须再次调用mq_notify以重新注册（如果需要的话），重新注册要放在从消息队列读出消息之前而不是之后。


## POSIX消息队列实例
- [01mq_open](01mq_open.c)
- [02mq_unlink](02mq_unlink.c)
- [03mq_getattr](03mq_getattr.c)
- [04mq_send](04mq_send.c)
- [05mq_receive](05mq_receive.c)
- [06mq_notify](06mq_notify.c)