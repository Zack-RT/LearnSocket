#ifndef _SPECIFIED_LEN_
#define _SPECIFIED_LEN_
#include <sys/types.h>
ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);

#endif