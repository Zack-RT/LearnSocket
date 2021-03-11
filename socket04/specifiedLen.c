#include <unistd.h>
#include <errno.h>

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