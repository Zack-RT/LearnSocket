#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <stdlib.h>

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