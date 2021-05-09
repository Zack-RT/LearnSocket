#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

typedef struct{
    char name[32];
    int age;
} STU;

int main()
{
    int shmid = shmget(1234, 0, 0);
    if (shmid < 0){
        ERR_EXIT("shmget");
    }
    printf("shmget success, msgid=%d\n", shmid);
    
    STU *p = shmat(shmid, NULL, 0);
    if(p == (void*)-1){
        ERR_EXIT("shmat");
    }
    printf("name=%s, age=%d\n", p->name, p->age);
    p->age = 20;
    
    shmdt(p);
    return 0;
}