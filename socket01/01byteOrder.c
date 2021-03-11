//验证大端排布与小端排布
//x86的机器一般为小端排布：低字节位于低地址
//MIPS、TCP/IP协议等为大端排布：低字节位于高地址
#include <stdio.h>
#include <arpa/inet.h>

int main(){
	unsigned long x =0x12345678;
	unsigned char *p = (unsigned char *)&x;
	printf("%x, %x, %x, %x\n", p[0], p[1], p[2], p[3]);
	
	//将一个long类型数据由本地主机字节序转换为网络字节序
	unsigned long y = htonl(x); 
	p = (unsigned char *)&y;
	
	printf("%x, %x, %x, %x\n", p[0], p[1], p[2], p[3]);
	
	return 0;
}
