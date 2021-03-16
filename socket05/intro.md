# socket编程（五）
## read、write与recv、send
- 字节流套接口(如tcp套接口)上的read和write函数所表现的行为不同于通常的文件IO。字节流套接口上的读或写输入或输出的字节数可能比要求的数量少，但这不是错误状况，原因是内核中套接口的缓冲区可能已达到了极限。此时所需的是调用者再次调用read或write函数，以输入或输出剩余的字节。
- recv和read相似，都可用来接收sockfd发送的数据，但recv比read多了一个参数，也就是第四个参数，它可以指定标志来控制如何接收数据。
    - 函数原型：ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
    - 若flags=0，则recv与read效果相同

## 实现readline解决粘包问题
- 读取遇到'\n'为止
```
ssize_t recv_peek(int sockfd, void *buf, size_t len){
    while(1){
        int ret = recv(sockfd, buf, len, MSG_PEEK); // 将数据从套接口缓冲区取出不删除
        if(ret == -1 && errno == EINTR)
            continue;
        return ret;
    }
}

ssize_t readline(int sockfd, void *buf, size_t maxline){
    int ret;
    int nread;
    char *bufp = buf;
    int nleft = maxline; // 遇到'\n'或者内容超过maxline返回
    while(1){
        ret = recv_peek(sockfd, bufp, nleft);
        if(ret < 0) // 失败直接返回
            return ret;
        if(ret == 0) //对方关闭
            return ret;
        
        nread = ret;
        int i;
        for(i = 0; i < nread; i++){
            if(bufp[i] == '\n'){ // 如果遇到'\n'取走
                ret = readn(sockfd, bufp, i+1);
                if(ret != i+1)
                    exit(EXIT_FAILURE);
                return ret;
            }
        }
        // 没'\n'说明后面还有内容
        if(nread > nleft) //这种情况不允许
            exit(EXIT_FAILURE);
        nleft -= nread;// 将收到的字符先读走，防止buf满了
        ret = readn(sockfd, bufp, nread);
        if(ret != nread) 
            exit(EXIT_FAILURE);
        bufp += nread;
        // 然后继续看看有没收到'\n'
    }
    return -1; //错误，不会从此处返回
}
```
### 可重入、不可重入函数
这种情况出现在多任务系统当中，在任务执行期间捕捉到信号并对其进行处理时，进程正在执行的指令序列就被信号处理程序临时中断。如果从信号处理程序返回，则继续执行进程断点处的正常指令序列，从重新恢复到断点重新执行的过程中，函数所依赖的环境没有发生改变，就说这个函数是可重入的，反之就是不可重入的。

在进程中断期间，系统会保存和恢复进程的上下文，然而恢复的上下文仅限于返回地址，cpu寄存器等之类的少量上下文，而函数内部使用的诸如全局或静态变量，buffer等并不在保护之列，所以如果这些值在函数被中断期间发生了改变，那么当函数回到断点继续执行时，其结果就不可预料了。
- 满足下面条件之一的多数是不可重入函数：
    - (1)使用了静态数据结构;
    - (2)调用了malloc或free;
    - (3)调用了标准I/O函数;标准io库很多实现都以不可重入的方式使用全局数据结构。 
    - (4)进行了浮点运算.许多的处理器/编译器中，浮点一般都是不可重入的 (浮点运算大多使用协处理器或者软件模拟来实现。

## getsockname
- 获取套接口本地地址与端口
- 原型
```
#include <sys/socket.h>
int getsockname(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
## getpeername
- 获取对方套接口的地址与端口，需要connect或者accept连接上的套接口使用
- 原型
```
#include <sys/socket.h>
int getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```
## gethostname
- 获取本地主机的名字
- 原型：
```
#include <unistd.h>
int gethostname(char *name, size_t len);
```
## gethostbyname
- 通过主机主机名获取所有ip地址
- 
```
#include <netdb.h>
struct hostent *gethostbyname(const char *name);
 struct hostent {
               char  *h_name;            /* official name of host */
               char **h_aliases;         /* alias list */
               int    h_addrtype;        /* host address type */
               int    h_length;          /* length of address */
               char **h_addr_list;       /* list of addresses */
           }
```
## gethostbyaddr
- 与gethostbyname类似