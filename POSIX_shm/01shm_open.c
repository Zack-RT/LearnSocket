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

typedef struct{
    char name[32];
    int age;
}STU;

int main()
{
    int shmid = shm_open("/xyz", O_CREAT | O_RDWR, 0666);
    if(shmid < 0){
        ERR_EXIT("shm_open");
    }
    printf("shm_open succ\n");
    int ret = ftruncate(shmid, sizeof(STU));
    if(ret < 0){
        ERR_EXIT("ftruncate");
    }
    struct stat buf;
    ret = fstat(shmid, &buf);
    if(ret < 0){
        ERR_EXIT("fstat");
    }
    printf("size=%ld, mode=%o\n", buf.st_size, buf.st_mode & 0777);
    close(shmid);
    return 0;
}