#include "shmfifo.h"

typedef struct{
    char name[32];
    int age;
}stu;

int main(void)
{
    shmfifo_t *fifo = shmfifo_init(1234, sizeof(stu), 3);
    shmfifo_destory(fifo);
    return 0;
}