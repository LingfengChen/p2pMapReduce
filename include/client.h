#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "csapp.h"

#define SIZEMAX 256
#define LOGIN 1
#define LOGOUT 2
#define KEEPALIVE 3

void* cmd(void*);
int Login(char conn_ip[SIZEMAX],int conn_port,int listenPort);
int Logout(char conn_ip[SIZEMAX],int conn_port,int listenPort);
void Listening(int listenPort);
void* handle(void*);
void ip_convert(SA addr,char*);

pthread_t beathearttid;
struct para{
	SA addr;
	int fd;
};

#endif /* __CLIENT_H__ */