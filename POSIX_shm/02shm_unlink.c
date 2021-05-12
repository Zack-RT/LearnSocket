#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

int main()
{
    int ret = shm_unlink("/xyz");
    if(ret < 0){
        ERR_EXIT("shm_unlink");
    }
    return 0;
}