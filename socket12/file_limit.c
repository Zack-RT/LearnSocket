#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>

int main(void)
{
    int ret = 0;
    struct rlimit rl;
    ret = getrlimit(RLIMIT_NOFILE, &rl);
    if (ret < 0){ return -1; }
    printf("current:%lu\nmax:%lu\n", rl.rlim_cur, rl.rlim_max);
    rl.rlim_cur = 2048;
    rl.rlim_max = 2048;
    ret = setrlimit(RLIMIT_NOFILE, &rl); //更改仅对当前进程与子进程生效
    if (ret < 0){ return -1; }
    ret = getrlimit(RLIMIT_NOFILE, &rl);
    printf("current:%lu\nmax:%lu\n", rl.rlim_cur, rl.rlim_max);
    return 0;
}   