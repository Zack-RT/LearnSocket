#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <sys/select.h>
#include <time.h>
#include <fcntl.h>
#include "socketutils.h"

ssize_t readn(int fd, void *buf, size_t count){
    size_t nleft = count;
    ssize_t nread;
    char *bufp = (char *)buf;
    while(nleft > 0){
        nread = read(fd, bufp, nleft);
        if(nread < 0){
            if(errno == EINTR) // 若是系统中断导致不算错误
                continue;
            else
                return -1;
        }
        else if(nread == 0){ // 对等放关闭，退出
            return count - nleft;
        }
        else{
            bufp += nread;
            nleft -= nread;
        }
    }
    return count;
}

ssize_t writen(int fd, const void *buf, size_t count){
    size_t nleft = count;
    ssize_t nwriten;
    char *bufp = (char *)buf;
    while(nleft > 0){
        nwriten = write(fd, bufp, nleft);
        if(nwriten < 0){
            if(errno == EINTR) // 若是系统中断导致不算错误
                continue;
            else
                return -1;
        }
        else if(nwriten == 0){ // 对于write返回0认为什么都没发生
            continue;
        }
        else{
            bufp += nwriten;
            nleft -= nwriten;
        }
    }
    return count;
}

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

// 读超时检测函数，不包含读操作
// @fd:文件描述符
// @wait_seconds:等待超时秒数，如果为0表示不检测超时
// @成功（未超时）返回0，失败返回1，且errno = ETIMEOUT
int read_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if(wait_seconds > 0){
        fd_set read_fdset;
        struct timeval timeout;

        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do{
            ret = select(fd+1, &read_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINTR);

        if(ret == 0){ // select超时返回0
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if(ret == 1){ // 加入的fd产生可读事件
            ret = 0;
        }
        return ret;
    }
    return -1;
}

// 与read_timeout大同小异
int write_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if(wait_seconds > 0){
        fd_set write_fdset;
        struct timeval timeout;

        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do{
            ret = select(fd+1, NULL, &write_fdset, NULL, &timeout);
        }while(ret < 0 && errno == EINTR);

        if(ret == 0){ // select超时返回0
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if(ret == 1){ // 加入的fd产生可读事件
            ret = 0;
        }
        return ret;
    }
    return -1;
}

// 带超时的accept
// @fd: 套接字
// @addr: 输出参数,返回对方地址
// @wait_seconds: 等待超时秒数，0表示不检测超时
// 成功（未超时）返回已连接套接字，超时返回-1并且errno = ETIMEOUT
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret = 0;
    socklen_t addrlen = sizeof(struct sockaddr_in);

    if(wait_seconds > 0){
        fd_set accept_fdset;
        struct timeval timeout;
        FD_ZERO(&accept_fdset);
        FD_SET(fd, &accept_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do{
            ret = select(fd+1, &accept_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINTR);
        
        if(ret == -1){
            return -1;
        }
        else if(ret == 0){
            errno = ETIMEDOUT;
            return -1;
        }
    }

    if(addr != NULL){
        ret = accept(fd, (struct sockaddr*)addr, &addrlen);
    }
    else{
        ret = accept(fd, NULL, NULL);
    }
    if(ret == -1){
        exit(EXIT_FAILURE);
    }
    
    return ret; // 返回已连接套接字
}

// 设置文件IO为非阻塞模式
void activate_nonblock(int fd)
{
    int ret;
    int flag = fcntl(fd, F_GETFD);
    if(flag == -1){
        exit(EXIT_FAILURE);
    }
    flag |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flag);
    if(ret == -1){
        exit(EXIT_FAILURE);
    }
}

// activate_nonblock的逆操作
void deactivate_nonblock(int fd)
{
    int ret;
    int flag = fcntl(fd, F_GETFD);
    if(flag == -1){
        exit(EXIT_FAILURE);
    }
    flag &= ~O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flag);
    if(ret == -1){
        exit(EXIT_FAILURE);
    }
}

// 带超时的connect
// @fd: 套接字
// @addr: 输出参数,返回对方地址
// @wait_seconds: 等待超时秒数，0表示不检测超时
// 成功（未超时）返回已连接套接字，超时返回-1并且errno = ETIMEOUT
int connet_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds)
{
    int ret = 0;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if(wait_seconds > 0){
        activate_nonblock(fd);
    }
    ret = connect(fd, (const struct sockaddr *)addr, addrlen);
    if(ret < 0 && errno == EINPROGRESS){
        fd_set connet_fdset;
        struct timeval timeout;
        FD_ZERO(&connet_fdset);
        FD_SET(fd, &connet_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do{
            ret = select(fd+1, NULL, &connet_fdset, NULL, &timeout) ; // 一但连接建立，套接字就处于可写状态，属于写事件
        }while(ret < 0 && errno == EINTR);
        if(ret == 0){
            ret = -1;
            errno = ETIMEDOUT;
        }
        else if(ret < 0){
            return -1;
        }
        else if(ret == 1){
            // 返回1有两种情况：
            // 1.套接字成功建立连接
            // 2.套接字产生错误事件，此时errno不会记录，只能通过getsockopt来获取错误。
            int err;
            socklen_t socklen = sizeof(addr);
            int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
            if(sockoptret == -1){
                return -1;
            }
            if(err == 0){ // 第一种情况
                ret = 0;
            }
            else{ // 第二种情况
                errno = err;
                ret = -1;
            }
        }
    }
    if(wait_seconds > 0){
        deactivate_nonblock(fd); // 之前设成了非阻塞，要改回来
    }

    return ret;
}