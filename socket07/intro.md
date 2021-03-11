# socket编程（六）
## TCP回射客户/服务器

## TCP十个流协议
- TCP是基于字节流传输的，只维护发送出去多少，确认了多少，没有维护消息与消息之间的边界，因而可能导致粘包问题。
- 粘包问题的解决方法是在应用层维护消息边界。

## 僵进程与SIGCHLD信号
- signal(SIGCHLD, SIG_IGN)
- signal(SIGCHLD, handle_sigchld)
> 不推荐使用signal(SIGCHLD, SIG_IGN)