# system V 信号量

## 内容回顾
- 参考[IPC](../IPC/readme.md)中的信号量部分

## 信号量集数据结构
```
struct semid_ds {
    struct ipc_perm sem_perm;  /* Ownership and permissions */
    time_t          sem_otime; /* Last semop time */
    time_t          sem_ctime; /* Last change time */
    unsigned long   sem_nsems; /* No. of semaphores in set */
};
```

## 常用函数
```
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

int semget(key_t key, int nsems, int semflg);
int semctl(int semid, int semnum, int cmd, ...);
int semop(int semid, struct sembuf *sops, size_t nsops);
```
### semget
- 功能：用来创建和访问一个信号量集
- 原型
  - int semget(key_t key, int nsems, int semflg);
- 参数
  - key：信号即得名字
  - nsems：信号集中信号量得个数
  - semflg：由9个权限标志构成，与open的mode相同
- 返回值：成功返回一个非负整数，即信号量集的标识码，失败返回-1

## semctl
- 功能：用于控制信号量集
- 原型
  - int semctl(int semid, int semnum, int cmd, ...);
- 参数
  - semid：有semget返回的信号集标识码
  - semnum：信号集中信号量的序号
  - cmd：将要采取的动作（有三个可取值）
  - ...：最后一个参数根据命令的不同而不同
- 返回值：成功返回0，失败返回-1

cmd|说明
-|-
SETVAL|设置信号量中的信号量的计数值
GET_VAL|获取信号量中的信号量计数值
IPC_STAT|把semid_ds结构中的数据设置为信号集的当前关联值
IPC_SET|在进程有足够权限的前提下，把信号集的当前关联值设置为semid_ds数据结构中给出的值
IPC_RMID|删除信号集

### semop
- 功能：用来进行PV操作
- 原型
```
struct sembuf{
    unsigned short sem_num;  /* semaphore number */
    short          sem_op;   /* semaphore operation */
    short          sem_flg;  /* operation flags */
};
int semop(int semid, struct sembuf *sops, size_t nsops);
```
- 参数
  - semid：要操作的信号量的标识码
  - sops：指向一个结构数值的指针
  - nsops：信号量的个数
- 返回值：成功返回0，失败返回-1
- sembuf结构体
  - sem_num：是信号量的编号
  - sem_op：是信号量的一次PV操作时加减的数值，一般只会用到两个值，一个是“-1”，也就是P操作，等待信号量变得可用；另一个是“+1”，也就是V操作，发出信号量已经变得可用。
  - sem_flg：两个取值IPC_NOWAIT或SEM_UNDO