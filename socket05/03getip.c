#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

#define ERR_EXIT(msg) \
    { \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }

int main(void){
    char hostname[100];
    if(gethostname(hostname, sizeof(hostname)) < 0) // 获取主机名
        ERR_EXIT("gethostname");
    
    struct hostent *hp;
    if((hp = gethostbyname(hostname)) == NULL) // 通过主机名获取ip
        ERR_EXIT("gethostbyname");
    
    int i = 0;
    while (hp->h_addr_list[i] != NULL)
    {
        printf("%s\n", inet_ntoa(*(struct in_addr *)hp->h_addr_list[i]));
        i++;
    }
    return 0;
}
