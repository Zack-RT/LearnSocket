#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

typedef struct{
    char name[4];
    int age;
} STU;

int main(int argc, char **argv)
{
    if(argc < 2){
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
    }
    int fd = open(argv[1], O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(fd < 0){
        ERR_EXIT("open");
    }
    lseek(fd, sizeof(STU)*5-1, SEEK_SET); // 指针定位到39个字节处
    write(fd, "", 1); // 增加一个，共40个字节
    STU *p = (STU *)mmap(NULL, sizeof(STU)*5-1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(!p){
        ERR_EXIT("mmap");
    }
    char ch = 'a';
    for(int i = 0; i < 5; i++){
        memcpy((p+i)->name, &ch, 1);
        (p+i)->age = 20 + i;
        ch += 1;
    }
    printf("initialize over\n");
    munmap(p, sizeof(STU)*5);
    printf("exit...\n");

    return 0;
}