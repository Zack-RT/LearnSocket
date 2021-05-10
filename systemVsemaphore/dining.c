#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};

#define DELAY ((rand() % 5) + 1)

int semid;

int wait_1fork(int no)
{
    struct sembuf sb = {no, -1, 0};
    int ret = semop(semid, &sb, 1);
    if(ret < 0){
        ERR_EXIT("semop");
    }
    return ret;
}

int sem_v(int semid)
{
    struct sembuf sb = {0, 1, 0};
    int ret = semop(semid, &sb, 1);
    if(ret < 0){
        ERR_EXIT("semop");
    }
    return ret;
}

void wait_for_2fork(int no)
{
    int left = no;
    int right = (no + 1) % 5;
    struct sembuf buf[2] = {
        {left, -1, 0},
        {right, -1, 0}
    };
    semop(semid, buf, 2);
}

void free_2fork(int no)
{
    int left = no;
    int right = (no + 1) % 5;
    struct sembuf buf[2] = {
        {left, 1, 0},
        {right, 1, 0}
    };
    semop(semid, buf, 2);
}

void philosophere(int no)
{
    while (1)
    {
        // 不会死锁
        // printf("%d is thinking\n", no);
        // sleep(DELAY);
        // printf("%d is hungry\n", no);
        // wait_for_2fork(no); // 请求两把刀叉
        // printf("%d is eating\n", no);
        // sleep(DELAY); 
        // free_2fork(no); // 吃完饭后归还两把刀叉
        
        // 可能死锁
        int left = no;
        int right = (no + 1) % 5;
        printf("%d is thinking\n", no);
        sleep(DELAY);
        printf("%d is hungry\n", no);
        wait_1fork(left); 
        wait_1fork(right);
        printf("%d is eating\n", no);
        sleep(DELAY); 
        free_2fork(no);
    }
}

int main(int argc, char **argv)
{
    semid = semget(IPC_PRIVATE, 5, IPC_CREAT | 0666);
    if(semid < 0){
        ERR_EXIT("semget");
    }
    union semun su;
    su.val = 1;
    for(int i = 0; i < 5; i++){ // 5把刀叉 5个人
        semctl(semid, i, SETVAL, su);
    }
    int no = 0;
    pid_t pid;
    for(int i = 1; i < 5; i++){
        pid = fork();
        if(pid == -1){
            ERR_EXIT("fork");
        }
        if(pid == 0){
            no = i;
            break;
        }
    }
    philosophere(no);
    if(no == 0){
        for(int i = 1; i < 5; i++){
            wait(NULL);
        }
    }
    return 0;
}