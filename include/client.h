#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "csapp.h"

void* cmd(void*);
int Login(char conn_ip[SIZEMAX],int conn_port,int listenPort);
int Logout(char conn_ip[SIZEMAX],int conn_port,int listenPort);
void Listening(int listenPort);
void* handle(void*);
void ip_convert(SA addr,char*);
void jobSubmit(char conn_ip[SIZEMAX],int conn_port,int listenPort,char* program_name,char* data_name,char* output,int num);
void fileRun(char* program_name,char* data_name,char* output);

pthread_t beathearttid;
struct para{
	SA addr;
	int fd;
};

#endif /* __CLIENT_H__ */