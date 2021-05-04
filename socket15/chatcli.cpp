#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "pub.h"

char username[16];
USER_LIST clients_list;

void chat_cli(int);
void do_someone_login(message&);
void do_someone_logout(message&);
void do_getlist(int);
void do_chat(message&);
void parse_cmd(char*, int, sockaddr_in*);
bool sendmsgto(int, char*, char*);

int main(void)
{
    int sock;
    if((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0){
        ERR_EXIT("main socket");
    }
    chat_cli(sock);
    return 0;
}

void chat_cli(int sock)
{
    sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188U);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    sockaddr_in peeraddr;
    socklen_t peerlen = sizeof(peeraddr);
    message msg;
    while (1)
    {
        memset(username, 0, sizeof(username));
        printf("please input your name: ");
        fflush(stdout);
        scanf("%s", username);

        memset(&msg, 0, sizeof(msg));
        msg.cmd = htonl(C2S_LOGIN);
        strcpy(msg.body, username);

        sendto(sock, &msg, sizeof(msg), 0, (const sockaddr *)&servaddr, sizeof(servaddr));
        memset(&msg, 0, sizeof(msg));
        recvfrom(sock, &msg, sizeof(msg), 0, nullptr, nullptr);
        int cmd = ntohl(msg.cmd);
        if(cmd == S2C_ALREADY_LOGIN){
            printf("user %s already logined server, please use another username", username);
            continue;
        }
        else if(cmd == S2C_LOGIN_OK){
            printf("user %s has logined server\n", username);
            break;
        }
    }

    int count;
    recvfrom(sock, &count, sizeof(int), 0, nullptr, nullptr);
    int n = ntohl(count);
    printf("has %ud users logined server.\n", n);
    for(int i = 0; i < n; i++){
        user_info user;
        recvfrom(sock, &user, sizeof(user), 0, nullptr, nullptr);
        clients_list.push_back(user);
        in_addr tmp;
        tmp.s_addr = user.ip;
        printf("%d %s <-> %s:%d\n", i, user.username, inet_ntoa(tmp), user.port);
    }

    printf("\nCommands are:\n");
    printf("send <username> [msg]\n");
    printf("list\n");
    printf("exit\n");
    printf("\n");

    fd_set rset;
    FD_ZERO(&rset);
    int nready;
    int stdinfileno = fileno(stdin);

    while (1)
    {
        FD_SET(stdinfileno, &rset);
        FD_SET(sock, &rset);
        nready = select(sock+1, &rset, nullptr, nullptr, nullptr);
        if(nready == -1)
            ERR_EXIT("char_cli select");
        if(nready == 0)
            continue;
        if(FD_ISSET(sock, &rset)){
            memset(&msg, 0, sizeof(msg));
            recvfrom(sock, &msg, sizeof(msg), 0, (sockaddr *)&peeraddr, &peerlen);
            int cmd = ntohl(msg.cmd);
            switch (cmd)
            {
            case S2C_SOMEONE_LOGIN:
                do_someone_login(msg);
                break;
            case S2C_SOMEONE_LOGOUT:
                do_someone_logout(msg);
                break;
            case S2C_ONLINE_USER:
                do_getlist(sock);
                break;
            case C2C_CHAT:
                do_chat(msg);
                break;
            default:
                ERR_EXIT("chat_cli switch");
                break;
            }
        }
        if(FD_ISSET(stdinfileno, &rset)){
            char cmdline[100] = {0};
            if(fgets(cmdline, sizeof(cmdline), stdin) == NULL)
                break;
            if(cmdline[0] == '\n')
                continue;
            cmdline[strlen(cmdline)-1] = '\0';
            parse_cmd(cmdline, sock, &servaddr);
        }
    }
    
}

void parse_cmd(char *cmdline, int sock, sockaddr_in* servaddr)
{
    char cmd[10] = {0};
    char *p = strchr(cmdline, ' '); // 在一个串中查找给定字符的第一个匹配之处
    if(p){
        *p = '\0';
    }
    strcpy(cmd, cmdline);
    if(strcmp(cmd, "exit") == 0){
        message msg;
        memset(&msg, 0, sizeof(msg));
        msg.cmd = htonl(C2S_LOGOUT);
        strcpy(msg.body, username);
        if(sendto(sock, &msg, sizeof(msg), 0, (const sockaddr*)servaddr, sizeof(sockaddr)) < 0)
            ERR_EXIT("parse_cmd sendto");
        printf("user %s has logout server\n", username);
        exit(EXIT_SUCCESS);
    }
    else if(strcmp(cmd, "send") == 0){
        char peername[16] = {0};
        char msg[MSG_LEN] = {0};
        while(*p++ == ' ');
        char *p2;
        p2 = strchr(p, ' ');
        if(!p2){
            printf("bad command\n");
            printf("\nCommands are:\n");
            printf("send <username> [msg]\n");
            printf("list\n");
            printf("exit\n");
            printf("\n");
            return;
        }
        *p2 = '\0';
        strcpy(peername, p);
        while(*p2++ == ' ');
        strcpy(msg, p2);
        sendmsgto(sock, peername, msg);
    }
    else if(strcmp(cmd, "list") == 0){
        message msg;
        memset(&msg, 0, sizeof(msg));
        msg.cmd = htonl(C2S_ONLINE_USER);
        if(sendto(sock, &msg, sizeof(msg), 0, (const sockaddr*)servaddr, sizeof(sockaddr)) < 0)
            ERR_EXIT("parse_cmd sendto2");
    }
    else{
        printf("bad command\n");
        printf("\nCommands are:\n");
        printf("send <username> [msg]\n");
        printf("list\n");
        printf("exit\n");
        printf("\n");
    }
}

bool sendmsgto(int sock, char *name, char *msg)
{
    if(strcmp(name, username) == 0){
        printf("cannnot send message to self\n");
        return false;
    }
    USER_LIST::iterator search;
    for(search=clients_list.begin(); search != clients_list.end(); search++){
        if(strcmp(search->username, name) == 0){
            break;
        }
    }
    if(search == clients_list.end()){
        printf("user %s has not logined server\n", name);
        return false;
    }
    message m;
    memset(&m, 0, sizeof(m));
    m.cmd = htonl(C2C_CHAT);
    chat_msg cm;
    strcpy(cm.username, username);
    strcpy(cm.msg, msg);
    memcpy(m.body, &cm, sizeof(cm));

    sockaddr_in peeraddr;
    memset(&peeraddr, 0, sizeof(peeraddr));
    peeraddr.sin_family = AF_INET;
    peeraddr.sin_addr.s_addr = search->ip;
    peeraddr.sin_port = search->port;

    in_addr tmp;
    tmp.s_addr = search->ip;
    printf("sending message [%s] to user [%s] <-> %s:%d\n", msg, search->username, inet_ntoa(tmp), search->port);
    
    sendto(sock, &m, sizeof(m), 0, (const sockaddr*)&peeraddr, sizeof(peeraddr));
    return true;
}

void do_someone_login(message& msg)
{
    user_info *user = (user_info*)msg.body;
    in_addr tmp;
    tmp.s_addr = user->ip;
    printf("%s <-> %s:%d has logined server\n", user->username, inet_ntoa(tmp), user->port);
    clients_list.push_back(*user);
}

void do_someone_logout(message& msg)
{
    USER_LIST::iterator it;
    for(it=clients_list.begin(); it != clients_list.end(); it++){
        if(strcmp(it->username, msg.body) == 0)
            break;
    }
    if(it != clients_list.end()){
        clients_list.erase(it);
    }
    printf("user %s has logout server\n", msg.body);
}

void do_getlist(int sock)
{
    int count;
    recvfrom(sock, &count, sizeof(int), 0, nullptr, nullptr);
    count = ntohl(count);
    printf("has %d users logined server\n", count);

    clients_list.clear();
    for(int i = 0; i < count; i++){
        user_info user;
        recvfrom(sock, &user, sizeof(user), 0, nullptr, nullptr);
        clients_list.push_back(user);
        in_addr tmp;
        tmp.s_addr = user.ip;

        printf("%s <-> %s:%d\n", user.username, inet_ntoa(tmp), user.port);
    }
}

void do_chat(message& msg)
{
    chat_msg *cm = (chat_msg*)msg.body;
    printf("recv a msg [%s] from [%s]\n", cm->msg, cm->username);
}