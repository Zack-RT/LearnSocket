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

int sem_create(key_t key)
{
    int semid = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666); // 创建且仅能创建一次,0666中0为前缀，表示666是八进制
    if(semid  < 0){
        ERR_EXIT("semget");
    }
    return semid;
}

int sem_open(key_t key)
{
    int semid;
    semid = semget(key, 0, 0);
    if(semid < 0){
        ERR_EXIT("semget");
    }
    return semid;
}

int sem_setval(int semid, int val)
{
    union semun su;
    su.val = val;
    int ret = semctl(semid, 0, SETVAL, su);
    if (ret < 0){
        ERR_EXIT("semctl");
    }
    return ret;
}

int sem_getval(int semid)
{
    int ret = semctl(semid, 0, GETVAL, 0);
    if(ret < 0){
        ERR_EXIT("semctl");
    }
    return ret;
}

int sem_del(int semid)
{
    int ret = semctl(semid, 0, IPC_RMID, 0);
        if(ret < 0){
        ERR_EXIT("semctl");
    }
    return ret;
}

int sem_p(int semid)
{
    struct sembuf sb = {0, -1, 0};
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

int sem_getmode(int semid)
{
    union semun su;
    struct semid_ds sem;
    su.buf = &sem;
    int ret = semctl(semid, 0, IPC_STAT, su);
    if(ret < 0){
        ERR_EXIT("semctl");
    }
    printf("current permission is %o\n", su.buf->sem_perm.mode);
    return ret;
}

int sem_setmode(int semid, char *mode)
{
    union semun su;
    struct semid_ds sem;
    su.buf = &sem;

    int ret = semctl(semid, 0, IPC_STAT, su);
    if(ret < 0){
        ERR_EXIT("semctl");
    }
    printf("current permission is %o\n", su.buf->sem_perm.mode);
    sscanf(mode, "%o", (unsigned int *)&su.buf->sem_perm.mode);
    ret = semctl(semid, 0, IPC_SET, su);
    if(ret < 0){
        ERR_EXIT("semctl");
    }
    printf("permission updated...\n");
    return ret;
}

int semid = 0;

void print(char op_char)
{
    int pause_time;
    srand(getpid());
    int i;
    for(i = 0; i < 10; i++){
        //------------------临界区,X或O总是成对出现
        sem_p(semid);
        printf("%c", op_char);
        fflush(stdout);
        pause_time = rand()%2;
        sleep(pause_time); // 灵界区内，时间片不会轮转
        printf("%c", op_char);
        fflush(stdout);
        sem_v(semid);
        //------------------
        pause_time = rand()%2;
        sleep(pause_time);
    }
}
int main(int argc, char **argv)
{
    semid = sem_create(IPC_PRIVATE);
    pid_t pid = fork();
    if(pid == -1){
        ERR_EXIT("fork");
    }
    if(pid > 0){
        sem_setval(semid, 1);
        print('O');
        wait(NULL);
        sem_del(semid);
    }
    else{
        print('X');
    }
    return 0;
}