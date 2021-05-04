#ifndef __PUB_H
#define __PUB_H

#include <list>
#include <algorithm>

// Clients send to Server
const unsigned C2S_LOGIN = 0x01;
const unsigned C2S_LOGOUT = 0x02;
const unsigned C2S_ONLINE_USER = 0x03;

const unsigned MSG_LEN = 512U;

// Server send to Clients
const unsigned S2C_LOGIN_OK = 0x01;
const unsigned S2C_ALREADY_LOGIN = 0x02;
const unsigned S2C_SOMEONE_LOGIN = 0x03;
const unsigned S2C_SOMEONE_LOGOUT = 0x04;
const unsigned S2C_ONLINE_USER = 0x05;

// Clients send to Clients
const unsigned C2C_CHAT = 0x06;

struct message
{
    unsigned cmd; // 记录上面定义操作
    char body[MSG_LEN];
};

struct user_info
{
    char username[16];
    unsigned int ip;
    unsigned short port;
};

struct chat_msg
{
    char username[16];
    char msg[100];
};

using USER_LIST = std::list<user_info>;

#define ERR_EXIT(msg) \
    do{ \
        perror(msg); \
        exit(EXIT_FAILURE); \
    }while(0)

#endif