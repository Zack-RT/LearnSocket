# socket编程（十一）

## 套接字IO超时设置方法
- alarm设置闹钟的方式
- 套接字选项
  - SO_SNDTIMENO
  - SO_RCVTIMENO
- select

## 用select实现超时
- read_timeout函数封装
- write_timeout函数封装
- connect_timeout函数封装
- accept_timeout函数封装