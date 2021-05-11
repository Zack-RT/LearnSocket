#include "shmfifo.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <errno.h>

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

shmfifo_t *shmfifo_init(int key, int blksize, int blocks)
{
    shmfifo_t *fifo = (shmfifo_t *)malloc(sizeof(shmfifo_t));
    assert(fifo != NULL);
    memset(fifo, 0, sizeof(shmfifo_t));
    fifo->shmid = shmget(key, 0, 0);
    if(fifo->shmid < 0){ // 共享内存为创建
        int size = sizeof(shmhead_t) + blksize * blocks;
        fifo->shmid =  shmget(key, size, IPC_CREAT | 0666);
        if (fifo->shmid < 0){
            ERR_EXIT("shmget");
        }
        fifo->p_shm = (shmhead_t *)shmat(fifo->shmid, NULL, 0);
        if(fifo->p_shm == (shmhead_t *)-1){
            ERR_EXIT("shmat");
        }
        fifo->p_payload = (char *)(fifo->p_shm + 1); // 有效负载需要除去头部
        fifo->p_shm->blksize = blksize;
        fifo->p_shm->blocks = blocks;
        fifo->p_shm->rd_index = fifo->p_shm->wr_index = 0U;
        fifo->sem_mutex = sem_create(key); // 共享内存和信号量的key可以相同
        fifo->sem_full = sem_create(key + 1);
        fifo->sem_empty = sem_create(key + 2);

        sem_setval(fifo->sem_mutex, 1);
        sem_setval(fifo->sem_full, blocks);
        sem_setval(fifo->sem_empty, 0);
    }
    else{
        fifo->p_shm = (shmhead_t *)shmat(fifo->shmid, NULL, 0);
        if(fifo->p_shm == (shmhead_t *)-1){
            ERR_EXIT("shmat");
        }
        fifo->p_payload = (char *)(fifo->p_shm + 1);
        fifo->sem_mutex = sem_open(key); 
        fifo->sem_full = sem_open(key + 1);
        fifo->sem_empty = sem_open(key + 2);
    }
    return fifo;
}

void shmfifo_put(shmfifo_t *fifo, const void *buf) // 生产者
{
    sem_p(fifo->sem_full);
    sem_p(fifo->sem_mutex);
    memcpy(fifo->p_payload + (fifo->p_shm->wr_index * fifo->p_shm->blksize), \
        buf, fifo->p_shm->blksize);
    fifo->p_shm->wr_index = (fifo->p_shm->wr_index + 1) % fifo->p_shm->blocks;
    sem_v(fifo->sem_mutex);
    sem_v(fifo->sem_empty);
}

void shmfifo_get(shmfifo_t *fifo, void *buf)
{
    sem_p(fifo->sem_empty);
    sem_p(fifo->sem_mutex);
    memcpy(buf, fifo->p_payload + (fifo->p_shm->rd_index * fifo->p_shm->blksize), \
        fifo->p_shm->blksize);
    fifo->p_shm->rd_index = (fifo->p_shm->rd_index + 1) % fifo->p_shm->blocks;
    sem_v(fifo->sem_mutex);
    sem_v(fifo->sem_full);
}

void shmfifo_destory(shmfifo_t *fifo)
{
    sem_del(fifo->sem_mutex);
    sem_del(fifo->sem_full);
    sem_del(fifo->sem_empty);
    shmdt(fifo->p_shm);
    shmctl(fifo->shmid, IPC_RMID, 0);
    free(fifo);
}