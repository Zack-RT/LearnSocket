#ifndef __SOCKET_UTILS__
#define __SOCKET_UTILS__
#include <sys/types.h>
#include <netinet/in.h>

ssize_t readn(int fd, void *buf, size_t count);
ssize_t writen(int fd, const void *buf, size_t count);
ssize_t recv_peek(int sockfd, void *buf, size_t len);
ssize_t readline(int sockfd, void *buf, size_t maxline);

void activate_nonblock(int fd);
void deactivate_nonblock(int fd);
int read_timeout(int fd, unsigned int wait_seconds);
int write_timeout(int fd, unsigned int wait_seconds);
int accept_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);
int connet_timeout(int fd, struct sockaddr_in *addr, unsigned int wait_seconds);

#endif