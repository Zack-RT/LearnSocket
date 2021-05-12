#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */

#include <string.h>

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
    int shmid = shm_open("/xyz", O_RDONLY, 0);
    if(shmid < 0){
        ERR_EXIT("shm_open");
    }
    printf("shm_open succ\n");
    
    struct stat buf;
    int ret = fstat(shmid, &buf);
    if(ret < 0){
        ERR_EXIT("fstat");
    }

    STU *p = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, shmid, 0);
    if(p == MAP_FAILED){
        ERR_EXIT("mmap");
    }
    printf("name=%s, age=%d\n", p->name, p->age);
    close(shmid);
    return 0;
}
