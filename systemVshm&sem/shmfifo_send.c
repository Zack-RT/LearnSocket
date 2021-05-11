#include "shmfifo.h"
#include <string.h>
#include <stdio.h>

typedef struct{
    char name[32];
    int age;
}stu;

int main()
{
    shmfifo_t *fifo = shmfifo_init(1234, sizeof(stu), 3);
    stu s;
    for(int i = 0 ; i < 5; i++){
        memset(&s, 0, sizeof(s));
        s.age = 20 + i;
        s.name[0] = 'A' + i;
        shmfifo_put(fifo, &s);
        printf("send shm ok\n");
    }
    return 0;
}