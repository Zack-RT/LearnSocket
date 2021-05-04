# socket编程（十七）

## socketpair函数
- 功能：创建一个全双工的流管道
- 原型
```
#include <sys/types.h>
#include <sys/socket.h>
int socketpair(int domain, int type, int protocol, int sv[2]);
```
- 参数
  - domain：    协议家族
  - type：      套接字类型
  - protocol：  协议类型
  - sv：        套接字对
- 返回值： 成功为0，失败为-1

## sendmsg/recvmsg
- 原型
```
#include <sys/types.h>
#include <sys/socket.h>
ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
```

## UNIX域套接字传递描述符字