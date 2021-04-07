# socket编程（十）

## 本节内容
- close与shutdown的区别
- 进一步改进回射客户程序

## close与shutdown的区别
```
int shutdown(int sockfd, int how);
// man 2 shutdown:
// If how is SHUT_RD, further receptions will be disallowed.  If how is SHUT_WR,  further  transmis‐sions will be disallowed.  If how is SHUT_RDWR, further receptions and transmissions will be disallowed.
```
- close终止了数据传送的两个方向。
- shutdown可以选择的中止某个方向的数据传送或者终止数据传送的两个方向。
- how = SHUT_WR就可以保证对等方接收到一个EOF字符，而不管其他进程是否已经打开了套接字。而close不能保证，直到套接字引用计数减为0时才发送。也就是说直到所有的进程都关闭了套接字。

## 改进回射客户与服务程序
- 在stdin中输入EOF后客户端关闭套接字，但如果服务器回射信息因为某些原因延迟，此时由于client已经关闭套接字，服务器再往socket写入消息会返回SIG_PIPE信号，客户端也无法接受到回射信息。为了解决这个问题client可以使用shutdown将套接字的写入关闭，这样服务器回射的消息还能接收到。
- 