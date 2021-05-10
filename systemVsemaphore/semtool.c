#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>

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

void usage(void)
{
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "semtool -c\n");
    fprintf(stderr, "semtool -d\n");
    fprintf(stderr, "semtool -p\n");
    fprintf(stderr, "semtool -v\n");
    fprintf(stderr, "semtool -s <val>\n");
    fprintf(stderr, "semtool -g\n");
    fprintf(stderr, "semtool -f\n");
    fprintf(stderr, "semtool -m <mode>\n");
}

int main(int argc, char **argv)
{
    int opt = getopt(argc, argv, "cdpvs:gfm:");
    if(opt == '?'){
        exit(EXIT_FAILURE);
    }
    if(opt == -1){
        usage();
        exit(EXIT_FAILURE);
    }
    key_t key = ftok(".", 's'); // 通过路径和一个非零整数生成一个key值
    int semid;
    switch (opt)
    {
    case 'c':
        sem_create(key);
        break;
    case 'p':
        semid = sem_open(key);
        sem_p(semid);
        printf("current value=%d\n", sem_getval(semid));
        break;
    case 'v':
        semid = sem_open(key);
        sem_v(semid);
        printf("current value=%d\n", sem_getval(semid));
        break;
    case 'd':
        semid = sem_open(key);
        sem_del(semid);
        break;
    case 's':
        semid = sem_open(key);
        sem_setval(semid, atoi(optarg));
        break;
    case 'g':
        semid = sem_open(key);
        printf("current value=%d\n", sem_getval(semid));
        break;
    case 'f':
        semid = sem_open(key);
        sem_getmode(semid);
        break;
    case 'm':
        semid = sem_open(key);
        sem_setmode(semid, optarg);
        break;
    default:
        break;
    }
    return 0;
}