# socket编程（四）

## 流协议
- UDP是基于消息的传输服务，传输的是报文，有边界
- TCP是一个基于字节流的传输服务，无边界
> 如流水一样，无法区分水流与水流的边界。接收方每次读取时可能读取到一个数据块的部分数据
- 产生原因：
    - 套接口发送缓冲区
    - 因为MSS、MTU产生的分片
    - 拥塞控制、延迟发送等
- 解决方案：本质上还是要在应用层维护消息与消息的边界
    - 定长包，发送与接收都以定长的方式完成
    - 包尾加'\r''\n'如FTP，若包内信息本就含有'\r''\n'则需要转义
    - 包头加上包体的长度，包头定长传输
    - 更复杂的应用层的协议

## 封装readn与writen
- readn:读取定长包
```
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
            nleft -+ nread;
        }
    }
    return count;
}
```
- writen:发送定长包
```
ssize_t writen(int fd, const void *buf, size_t count){
    size_t nleft = count;
    ssize_t nwriten;
    char *bufp = (char *)buf;
    while(nleft > 0){
        nwriten = writen(fd, bufp, nleft);
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
            nleft -+ nwriten;
        }
    }
    return count;
}
```