#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "pub.h"

USER_LIST clients_list;

void do_login(message& msg, int sock, sockaddr_in *cliaddr);
void do_logout(message& msg, int sock, sockaddr_in *cliaddr);
void do_sendlist(int sock, sockaddr_in *cliaddr);
void char_srv(int sock);

int main(void)
{
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        ERR_EXIT("socket");
    }
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(sock, (const sockaddr*)&servaddr, sizeof(servaddr)) < 0){
        ERR_EXIT("bind");
    }

    char_srv(sock);
    return 0;
}

void char_srv(int sock)
{
    sockaddr_in cliaddr;
    socklen_t clilen = sizeof(cliaddr);
    int n;
    message msg;
    while (1)
    {
        memset(&msg, 0, sizeof(msg));
        n = recvfrom(sock, &msg, sizeof(msg), 0, (sockaddr*)&cliaddr, &clilen);
        if (n < 0){
            if (errno == EINTR) continue;
            ERR_EXIT("recv_from");
        }
        int cmd = ntohl(msg.cmd);
        switch (cmd)
        {
        case C2S_LOGIN:
            do_login(msg, sock, &cliaddr);
            break;
        case C2S_LOGOUT:
            do_logout(msg, sock, &cliaddr);
            break;
        case C2S_ONLINE_USER:
            do_sendlist(sock, &cliaddr);
            break;
        default:
            ERR_EXIT("switch cmd");
        }
    } 
}

void do_login(message& msg, int sock, sockaddr_in *cliaddr)
{
    user_info user;
    strcpy(user.username, msg.body); // C2S_LOGIN表示用户登入，body中存放用户名
    user.ip = cliaddr->sin_addr.s_addr;
    user.port = cliaddr->sin_port;
    // 查找用户是否已经登录
    bool exist = false;
    for(auto client: clients_list){
        if(strcmp(client.username, msg.body) == 0){
            exist = true;
            break;
        }
    }
    if(!exist){ // 没有登录
        printf("has a user login: %s <-> %s:%d\n", \
            user.username, inet_ntoa(cliaddr->sin_addr), htons(cliaddr->sin_port));
        clients_list.push_back(user);
        //登录成功应答
        message reply_msg;
        memset(&reply_msg, 0, sizeof(reply_msg));
        reply_msg.cmd = htonl(S2C_LOGIN_OK);
        sendto(sock, &reply_msg, sizeof(reply_msg), 0, (const sockaddr *)cliaddr, sizeof(sockaddr));
        // 发生在线人数
        int count = htonl((uint32_t)clients_list.size());
        sendto(sock, &count, sizeof(reply_msg), 0, (const sockaddr *)cliaddr, sizeof(sockaddr));
        printf("sending user list information to: %s <-> %s:%d\n", \
            user.username, inet_ntoa(cliaddr->sin_addr),  htons(cliaddr->sin_port));
        // 发送在线列表
        for(auto client: clients_list){
            sendto(sock, &client, sizeof(user_info), 0, (const sockaddr *)cliaddr, sizeof(sockaddr));
        }
        // 向其他用户通知有新用户登录
        for(auto client: clients_list){
            if(strcmp(client.username, user.username) == 0) continue; // 对当前用户忽略
            sockaddr_in peeraddr;
            memset(&peeraddr, 0, sizeof(peeraddr));
            peeraddr.sin_family = AF_INET;
            peeraddr.sin_port = client.port;
            peeraddr.sin_addr.s_addr = client.ip;
            msg.cmd = htonl(S2C_SOMEONE_LOGIN);
            memcpy(msg.body, &user, sizeof(user));
            if(sendto(sock, &msg, sizeof(msg), 0, (const sockaddr *)&peeraddr, sizeof(peeraddr)) < 0){
                ERR_EXIT("sendto");
            }
        }
    }
    else{ // 用户已登录，不能重复登录
        printf("user %s has already logined\n", msg.body);
        message reply_msg;
        memset(&reply_msg, 0, sizeof(reply_msg));
        reply_msg.cmd = htonl(S2C_ALREADY_LOGIN);
        sendto(sock, &reply_msg, sizeof(reply_msg), 0, (const sockaddr *)cliaddr, sizeof(sockaddr));
    }
}

void do_logout(message& msg, int sock, sockaddr_in *cliaddr)
{
    printf("has a user logout: %s <-> %s:%d\n", msg.body, inet_ntoa(cliaddr->sin_addr), cliaddr->sin_port);
    USER_LIST::iterator it;
    for(it=clients_list.begin(); it != clients_list.end(); it++){
        if(strcmp(it->username, msg.body) == 0)
            break;
    }
    if(it != clients_list.end()){
        clients_list.erase(it);
    }
    // 向其他登录的客户端通知
    for(it=clients_list.begin(); it != clients_list.end(); it++){
        if(strcmp(it->username, msg.body) == 0) 
            continue;
        sockaddr_in peeraddr;
        memset(&peeraddr, 0, sizeof(peeraddr));
        peeraddr.sin_family = AF_INET;
        peeraddr.sin_port = it->port;
        peeraddr.sin_addr.s_addr = it->ip;
        msg.cmd = htonl(S2C_SOMEONE_LOGOUT);
        if(sendto(sock, &msg, sizeof(msg), 0, (const sockaddr*)&peeraddr, sizeof(peeraddr)) < 0)
            ERR_EXIT("sendto");
    }
}

void do_sendlist(int sock, sockaddr_in *cliaddr)
{
    message msg;
    msg.cmd = htonl(S2C_ONLINE_USER);
    sendto(sock, &msg, sizeof(msg), 0, (const sockaddr*)cliaddr, sizeof(sockaddr));
    int count = htonl((int)clients_list.size());
    sendto(sock, &count, sizeof(int), 0, (const sockaddr*)cliaddr, sizeof(sockaddr));
    for(auto client: clients_list){
        sendto(sock, &client, sizeof(user_info), 0, (const sockaddr*)cliaddr, sizeof(sockaddr));
    }
}