#ifndef __SERVER_H__
#define __SERVER_H__

#include "csapp.h"

#define SIZEMAX 256
#define TABLESIZE 100
#define MAXRETRY 5

#define LOGIN 1
#define LOGOUT 2
#define KEEPALIVE 3

#define ONLINE 1
#define EMPTY -1

struct clientNode{
	SA addr;
	pthread_t tid;
	int status;
};
struct clientNode Table[TABLESIZE];

void* cmd();
void Listening(int listenPort);
void* handle(void*);
void *keepAlive(void*);
int tableAdd(SA addr);
int tableRemove(SA addr);
int saEqual(SA addr1,SA addr2);

void ip_convert(SA addr,char*);

pthread_t beathearttid;
struct para{
	SA addr;
	int fd;
};

#endif /* __SERVER_H__ */