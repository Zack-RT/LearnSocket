//点分十进制与32位整数间的转换
#include <stdio.h>
#include <arpa/inet.h>

int main(){
	unsigned long address = inet_addr("192.168.0.100");
	printf("%u\n", ntohl(address)); //点分十进制转换为32位整数
	
	struct in_addr ipaddr;
	ipaddr.s_addr = address;
	printf("%s\n", inet_ntoa(ipaddr));//三十二位整数转换回点分十进制
	return 0;
}
