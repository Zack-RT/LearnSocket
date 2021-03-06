# System V消息队列
## 消息队列
- 消息队列提供了一个从一个进程向另外一个进程发送一块数据的方法
- 每个数据块都被认为是有一个类型，接收者进程接收的数据块可以有不同的类型值
- 消息队列也有管道一样的不足，就是每个消息的最大长度是有上限的（MSGMAX），每个消息队列的总的字节数是有上限的（MSGMNB），系统上消息队列的总数也有一个上限（MSGMNI），可通过cat /proc/sys/kernel/[msgmax or msgmnb or msgmni]查询

## IPC对象数据结构
- 内核为每个IPC对象维护一个数据结构
```
// man 2 msgctl
struct ipc_perm {
    key_t          __key;       /* Key supplied to msgget(2) */
    uid_t          uid;         /* Effective UID of owner */
    gid_t          gid;         /* Effective GID of owner */
    uid_t          cuid;        /* Effective UID of creator */
    gid_t          cgid;        /* Effective GID of creator */
    unsigned short mode;        /* Permissions */
    unsigned short __seq;       /* Sequence number */
};
```

## 消息队列结构
```
// man 2 msgctl
struct msqid_ds {
    struct ipc_perm msg_perm;     /* Ownership and permissions */
    time_t          msg_stime;    /* Time of last msgsnd(2) */
    time_t          msg_rtime;    /* Time of last msgrcv(2) */
    time_t          msg_ctime;    /* Time of last change */
    unsigned long   __msg_cbytes; /* Current number of bytes in
                                    queue (nonstandard) */
    msgqnum_t       msg_qnum;     /* Current number of messages
                                    in queue */
    msglen_t        msg_qbytes;   /* Maximum number of bytes
                                    allowed in queue */
    pid_t           msg_lspid;    /* PID of last msgsnd(2) */
    pid_t           msg_lrpid;    /* PID of last msgrcv(2) */
};
```

## 消息队列在内核中的表示
![消息队列在内核中的表示](mdimg/QQ截图20210505162713.png)

## 消息队列函数
```
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int msgget(key_t key, int msgflg);
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp,
               int msgflg);
```

### msgget
- 判断流程图
![msgget](mdimg/QQ截图20210505231838.png)
- 功能：用来创建和访问一个消息队列
- @param key:某个消息队列的名字
- @param msgflg:由九个权限标志构成，它们的用法和创建文件时使用的mode模式标志是一样的。
- 返回值：成功返回一个非负整数，失败返回-1

### msgsnd
- 功能：把一条消息添加到消息队列当中
- @param msqid:由msgget返回的消息队列表示码
- @param msgp:只想准备发送信息的指针
- @param msgsz:msgp指向的消息的长度值，不包含保存信息类型的那个long int
- @param msgflg:控制当前消息队列满或者达到系统值上限时的行为
- 返回值：成功返回0，失败返回-1
- msgflg=IPC_NOWAIT表示队列满不等待，返回EAGAIN错误。
- 消息结构在两方面收到制约。首先，它必须小于系统规定的上限值MSGMAX；其次，它必须以一个long int长整数开始，接收者将利用这个长整数确定消息的类型
- 消息结构参考形式如下：
```
struct msgbuf{
    long mtype;
    char mtext[1];
}
```

### msgrcv
- msgid：由msgget函数返回的消息队列标识码
- msggp：是一个指针，指针指向准备接受的消息
- msgsz：是msgp指向的消息长度，这个长度不包含保存消息类型的那个long int长整型
- msgtype：可以实现接受优先级的简单形式
- msgflg：控制着队列中没有相应类型的消息可供接受时函数的行为
- 返回值：成功返回实际接受到缓冲区的字节数，失败返回-1
- msgtype=0返回队列第一条信息
- msgtype>0返回队列第一条类型等于msgtype的信息
- msgtype<0返回队列第一条类型绝对值小于等于msgtype的信息
- msgflg=IPC_NOWAIT，队列没有可读消息时不等待直接返回，errno=ENOMSG
- msgflg=MSG_NOERROR，消息大小超过msgsz时会被截断
- msgtype>0 && msgflg=MSC_EXCEPT，接受类型不等于msgtype的第一条消息

## 用消息队列实现回射客户服务器程序
![figure2](mdimg/QQ截图20210507142237.png)
- 服用消息队列，通过指定type来区分消息的源与目的
- 有可能产生死锁：如果有大量的客户端将消息队列填满，服务器将无法回射，也无法进一步读取。
![figure3](mdimg/QQ截图20210507151945.png)
- 通过多个队列实现可避免死锁