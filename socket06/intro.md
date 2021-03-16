# socket编程（七）
## TCP11种状态
### 连接建立三次握手、连接终止四次挥手
从client发起连接请求到结束：
|  |  |  |  |  | |  | |
| - | - | - | - | - | - | - | - | 
| **client** | | SYN_SENT | ESTABLISHED|...| FIN_WAIT_1 | FIN_WAIT2 | TIME_WAIT |
| **server** |LISTEN|SYN_RCVD|ESTABLISHED|...|CLOSE_WAIT|LAST_ACK|CLOSED|

</br>

还有一个特殊的状态：CLOSING。当双方同时发起close后都进入FIN_WAIT_1，各自收到对方的FIN包文返回ACK，然后各自进入CLOSING状态，收到对方的ACK后进入TIME_WAIT

|  |  |  |  |  | |  | |
| - | - | - | - | - | - | - | - | 
| **client** | | SYN_SENT | ESTABLISHED|...| FIN_WAIT_1 | CLOSING | TIME_WAIT |
| **server** |LISTEN|SYN_RCVD|ESTABLISHED|...|FIN_WAIT_1|CLOSING|TIME_WAIT|

</br>

## SIGPIPE
- 往一个已经结束FIN的套接字中写数据是允许的，FIN仅仅代表对方不再发送数据
- 如果对方的进程已经不存在了，还写数据，则TCP会返回一个RST段，如果还调用write则会产生SIGPIPE信号，对于这个信号的处理我们通常忽略即可。