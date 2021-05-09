# systemV 共享内存

## 数据结构
```
struct shmid_ds {
    struct ipc_perm shm_perm;    /* Ownership and permissions */
    size_t          shm_segsz;   /* Size of segment (bytes) */
    time_t          shm_atime;   /* Last attach time */
    time_t          shm_dtime;   /* Last detach time */
    time_t          shm_ctime;   /* Last change time */
    pid_t           shm_cpid;    /* PID of creator */
    pid_t           shm_lpid;    /* PID of last shmat(2)/shmdt(2) */
    shmatt_t        shm_nattch;  /* No. of current attaches */
    ...
};
```

## 相关函数

```
#include <sys/ipc.h>
#include <sys/shm.h>

int shmget(key_t key, size_t size, int shmflg); // 创建或访问
void *shmat(int shmid, const void *shmaddr, int shmflg); // 映射共享内存段到进程空间
int shmdt(const void *shmaddr); // 解除映射
int shmctl(int shmid, int cmd, struct shmid_ds *buf); // 控制函数
```

### shmget
- 功能：用来创建共享内存
- 原型：
  - int shmget(key_t key, size_t size, int shmflg); 
- 参数
  - key：这个共享内存段的名字
  - size：共享内存大小
  - shmflg：由九个权限标志构成，用法与创建文件时使用的mode模型标志相同
- 返回值：成功返回一个非负整数，即该共享内存段的表示码，失败返回-1

### shmat
- 功能：将共享内存段连接到进程地址空间
- 原型：
  - void *shmat(int shmid, const void *shmaddr, int shmflg); 
- 参数：
  - shmid：共享内存id
  - shmaddr：指定连接的地址
  - shmflg：它的两个可能取值是SHM_RND和SHM_RDONLY
- 返回值：成功返回一个指针，指向共享内存的第一个接，失败返回-1
- shmaddr为NULL，核心自动选择一个地址
- shmaddr不为NULL且shmilg无SHM_RND标记，则以shmaddr为连接地址。
- shmaddr不为NULL且shmflg设置了SHM_RND标记，则连接的地址会自动向下调整为SHMLBA的整数倍。公式：shmeddr-(shmaddr % SHMLBA)
- shmfg=SHM_RDONLY，表示连接操作用来只读共享内存

### shmdt
- 功能：将共享内存段与当前进程脱离
- 原型
  - int shmdt(const void *shmaddr);
- 返回值：成功返回0，失败返回-1
- 注意：将共享内存段与当前进程脱离不等于删除共享内存段

### shmctl
- 功能：用来创建和访问一个消息队列
- 原型
  - int shmctl(int shmid, int cmd, struct shmid_ds *buf); 
- 参数
  - shmid：由shmget返回的共享内存id
  - cmd：将要采取的动作，见下表
  - buf：指向一个保存着共享内存模式状态和访问权限的数据结构
- 返回值：成功返回0，失败返回-1

cmd|说明
-|-
IPC_STAT|把shmid_ds结构中的数据设置为共享内存的当前关联值
IPC_SET|在进程有足够权限的前提下，把共享内存的当前值  设置为shm_ds数据结构中给出的值
IPC_RMID|删除共享内存段