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
        shmfifo_get(fifo, &s);
        printf("name=%s, age=%d\n", s.name, s.age);
    }
    return 0;
}