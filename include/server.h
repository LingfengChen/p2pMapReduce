#ifndef __SERVER_H__
#define __SERVER_H__

#include "csapp.h"

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
void selectNode(int num,struct clientNode Task[SIZEMAX],int* numActual);
int allocateTask(int taskid,struct clientNode client,char* program_name,char* data_name,char* output);
void fileSplit(char* file_name,int num);
void ip_convert(SA addr,char*);

pthread_t beathearttid;
struct para{
	SA addr;
	int fd;
};

#endif /* __SERVER_H__ */