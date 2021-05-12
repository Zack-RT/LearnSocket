#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)


int main(void)
{
    int ret = mq_unlink("/abc");
    if(ret < 0){
        ERR_EXIT("mq_unlink");
    }
    return 0;   
}