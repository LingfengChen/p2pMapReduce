#include <stdio.h>
#include "csapp.h"
#include "string.h"
#include <pthread.h>
#include "stdlib.h"
#include "server.h"

void* cmd()
{
	char input[SIZEMAX];
	char ips[SIZEMAX];
	int i=0;
    while(1)
    {
      	fgets(input,SIZEMAX,stdin);
      	input[strlen(input)-1]='\0';
      	if(!strcmp(input,"checktable"))
      	{
      		printf("___________________________\n");
      		printf("These Users are online:\n");
      		for(i=0;i<TABLESIZE;i++)
      		{
      			if(Table[i].status!=EMPTY)
      			{
      				ip_convert(Table[i].addr,ips);
      				printf("%s:%d\n",ips,ntohs(Table[i].addr.sin_port));
      			}
      		}
      		printf("___________________________\n");
      	}
    }
    return;
}

void Listening(int listenPort)
{
    SA clientaddr;
    pthread_t tid;
    int listenfd, connfd, clientlen, optval, serverPort, i , connPort;
    optval=1;
    struct para* Args;
    listenfd = Open_listenfd(listenPort);
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval , sizeof(int));
    while(1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (struct sockaddr *)&clientaddr, &clientlen);
        Args=(struct para*)Malloc(sizeof(struct para));
        Args->addr=clientaddr;
        Args->fd=connfd;
        Pthread_create(&tid,NULL,handle,Args);
        pthread_detach(tid);
    }
}

void *keepAlive(void *Args)
{
	SA clientaddr=*((SA*)Args);
	int fd;
	int num;
	int buf1[SIZEMAX];
	buf1[0]=KEEPALIVE;
	buf1[1]=ntohs(clientaddr.sin_port);
	rio_t client;
	int retry=0;
	while(1)
	{
		if(retry>MAXRETRY)
		{
			printf("retry time is over\n");
			tableRemove(clientaddr);
			return;
		}
		sleep(3);
		if ((fd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
		{
			printf("fd error\n");
			return;
		}
		if (connect(fd, (struct sockaddr *) &clientaddr, sizeof(SA)) < 0)
		{
			printf("connection error retry %d\n",retry);
			retry++;
			close(fd);
			continue;
		}
		if(fd<=0)
		{
			printf("fd < 0 retry %d\n",retry);
			retry++;
			sleep(5);
			close(fd);
			continue;
		}

		num=Rio_writen(fd,(char*)buf1,2*sizeof(int));
		if(num<=0)
		{
			printf("can not write retry %d\n",retry);
			retry++;
			sleep(5);
			close(fd);
			continue;
		}
		rio_readinitb(&client, fd);
		num = rio_readlineb(&client, buf1, sizeof(int));
		if(num<=0)
		{
			printf("can not read retry %d\n",retry);
			retry++;
			sleep(5);
			close(fd);
			continue;
		}
		close(fd);
		retry=0;
	}
}

void* handle(void *Args)
{
	SA clientaddr;
	int clientfd;
	int ret;
	clientaddr=((struct para*)Args)->addr;
	clientfd=((struct para*)Args)->fd;
	free(Args);
	int buf1[SIZEMAX];
	int buf2[SIZEMAX];
	char file_name[SIZEMAX];
	Rio_readp(clientfd,buf1,2*sizeof(int));
	clientaddr.sin_port=htons(buf1[1]);
	if(buf1[0]==LOGIN)
	{
		printf("Login request\n");
		ret=tableAdd(clientaddr);
		Rio_writep(clientfd,&ret,sizeof(int));
	}
	else if(buf1[0]==LOGOUT)
	{
		printf("Logout request\n");
		ret=tableRemove(clientaddr);
		Rio_writep(clientfd,&ret,sizeof(int));
	}
	else 

	close(clientfd);
}

int tableAdd(SA addr)
{
	pthread_t tid;
	int i=0;
	SA* Args;
	for(i=0;i<TABLESIZE;i++)
	{
		if(Table[i].status==EMPTY)
		{
			Args=(SA*)Malloc(sizeof(SA));
			*Args=addr;
        	Pthread_create(&tid,NULL,keepAlive,Args);
        	pthread_detach(tid);
        	Table[i].addr=addr;
			Table[i].status=ONLINE;
			Table[i].tid=tid;
			return i;
		}
	}
	return -1;
}
int tableRemove(SA addr)
{
	int i=0;
	for(i=0;i<TABLESIZE;i++)
	{
		if(Table[i].status!=EMPTY)
		{
			if(saEqual(addr,Table[i].addr))
			{
				Table[i].status=EMPTY;
				return i;
			}
		}
	}
	return -1;
}

int saEqual(SA addr1,SA addr2)
{
	if((addr1.sin_addr.s_addr==addr2.sin_addr.s_addr)&&(addr1.sin_port==addr2.sin_port))
		return 1;
	else
		return 0;
}

void ip_convert(SA addr,char* addrs)
{
	int ip4=addr.sin_addr.s_addr;
	int ip[4];
	ip[0]=ip4%256;
	ip[1]=(ip4/256)%256;
	ip[2]=(ip4/256/256)%256;
	ip[3]=(ip4/256/256/256)%256;
	sprintf(addrs,"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
}
