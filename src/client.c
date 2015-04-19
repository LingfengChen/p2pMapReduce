#include <stdio.h>
#include "csapp.h"
#include "string.h"
#include <pthread.h>
#include "stdlib.h"
#include "client.h"

void* cmd(void* args)
{
	int listenPort=*(int*)args;
	free(args);
	char input[SIZEMAX];
    char conn_ip[SIZEMAX];
    int conn_port;
	while(1)
    {
      	fgets(input,SIZEMAX,stdin);
      	input[strlen(input)-1]='\0';
      	if(!strcmp(input,"login"))
      	{
          	printf("login...\n");
          	scanf("%s %d",conn_ip,&conn_port);
          	printf("ip is %s port is %d\n",conn_ip,conn_port);
          	Login(conn_ip,conn_port,listenPort);
      	}
      	else if(!strcmp(input,"logout"))
      	{
          	printf("logout...\n");
          	Logout(conn_ip,conn_port,listenPort);
      	}
      	else
      	{
        	printf("Wrong cmd\n");
      	}
    }
}

int Login(char conn_ip[SIZEMAX],int conn_port,int listenPort)
{
	int buf1[SIZEMAX];
	int connfd;
	int ret;
	buf1[0]=LOGIN;
	buf1[1]=listenPort;
	connfd=Open_clientfd(conn_ip, conn_port);
	Rio_writep(connfd,buf1,2*sizeof(int));
	Rio_readp(connfd,&ret,sizeof(int));
	return ret;
}

int Logout(char conn_ip[SIZEMAX],int conn_port,int listenPort)
{
	int buf1[SIZEMAX];
	int connfd;
	int ret;
	buf1[0]=LOGOUT;
	buf1[1]=listenPort;
	connfd=Open_clientfd(conn_ip, conn_port);
	Rio_writep(connfd,buf1,2*sizeof(int));
	Rio_readp(connfd,&ret,sizeof(int));
	return ret;
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

void* handle(void *Args)
{
	SA clientaddr;
	int clientfd;
	clientaddr=((struct para*)Args)->addr;
	clientfd=((struct para*)Args)->fd;
	free(Args);
	int buf1[SIZEMAX];
	int buf2[SIZEMAX];
	char program_name[SIZEMAX];
	char data_name[SIZEMAX];
	Rio_readp(clientfd,buf1,2*sizeof(int));
	clientaddr.sin_port=htons(buf1[1]);
	if(buf1[0]==KEEPALIVE)
	{
		Rio_writep(clientfd,buf1,sizeof(int));
	}
	close(clientfd);
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
