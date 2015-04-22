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
    char filename[SIZEMAX];
    char program_name[SIZEMAX];
    char data_name[SIZEMAX];
    char output[SIZEMAX];
    int conn_port;
    int num=0;
	while(1)
    {
      	fgets(input,SIZEMAX,stdin);
      	input[strlen(input)-1]='\0';
      	if(!strcmp(input,"login"))
      	{
          	scanf("%s %d",conn_ip,&conn_port);
          	printf("login to %s : %d\n",conn_ip,conn_port);
          	Login(conn_ip,conn_port,listenPort);
      	}
      	else if(!strcmp(input,"logout"))
      	{
      		printf("logout from %s : %d\n",conn_ip,conn_port);
          	Logout(conn_ip,conn_port,listenPort);
      	}
      	else if(!strcmp(input,"download"))
      	{
      		scanf("%s",filename);
      		printf("download %s from %s : %d\n",filename,conn_ip,conn_port);
      		FileRecv(conn_ip,conn_port,listenPort,filename);
      	}
      	else if(!strcmp(input,"upload"))
      	{
      		scanf("%s",filename);
      		printf("upload %s to %s : %d\n",filename,conn_ip,conn_port);
      		FileSend(conn_ip,conn_port,listenPort,filename);
      	}
      	else if(!strcmp(input,"checkrun"))
      	{
      		fileRun("count.py","mapreduce.html","a.txt");
      	}
      	else if(!strcmp(input,"submit"))
      	{
      		scanf("%s %s %s %d",program_name,data_name,output,&num);
      		jobSubmit(conn_ip,conn_port,listenPort,program_name,data_name,output,num);
      	}
      	else
      	{
      		;
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
	close(connfd);
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
	close(connfd);
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
	int ret;
	int taskid;
	clientaddr=((struct para*)Args)->addr;
	clientfd=((struct para*)Args)->fd;
	free(Args);
	int buf1[SIZEMAX];
	int buf2[SIZEMAX];
	char cmd[SIZEMAX];
	char program_name[SIZEMAX];
	char data_name[SIZEMAX];
	char file_name[SIZEMAX];
	char output[SIZEMAX];
	Rio_readp(clientfd,buf1,2*sizeof(int));
	clientaddr.sin_port=htons(buf1[1]);
	if(buf1[0]==KEEPALIVE)
	{
		Rio_writep(clientfd,buf1,sizeof(int));
	}
	else if(buf1[0]==DOWNLOAD)
	{
		Rio_readp(clientfd,file_name,SIZEMAX);
		ret=fileSend(clientfd,file_name);
	}
	else if(buf1[0]==UPLOAD)
	{
		Rio_readp(clientfd,file_name,SIZEMAX);
		ret=fileRecv(clientfd,file_name);
	}
	else if(buf1[0]==ALLOCATE)
	{
		printf("Job Allocation\n");
		taskid=buf1[1];
		Rio_readp(clientfd,program_name,SIZEMAX);
		Rio_readp(clientfd,data_name,SIZEMAX);
		Rio_readp(clientfd,output,SIZEMAX);
		printf("%s %s %s\n",program_name,data_name,output);
		fileRecv(clientfd,program_name);
		fileRecv(clientfd,data_name);
		fileRun(program_name,data_name,output);
		fileSend(clientfd,output);
		sprintf(cmd,"rm %s",output);
		system(cmd);
		sprintf(cmd,"rm %s",data_name);
		system(cmd);
		sprintf(cmd,"rm %s",program_name);
		system(cmd);
	}
	close(clientfd);
}

void fileRun(char* program_name,char* data_name,char* output)
{
	char cmd[SIZEMAX];
	strcpy(cmd,"python ");
	strcat(cmd,program_name);
	strcat(cmd," <");
	strcat(cmd,data_name);
	strcat(cmd,"|sort");
	strcat(cmd," >");
	strcat(cmd,output);
	system(cmd);
}

void jobSubmit(char conn_ip[SIZEMAX],int conn_port,int listenPort,char* program_name,char* data_name,char* output,int num)
{
	int buf1[SIZEMAX];
	int connfd;
	int ret;
	buf1[0]=SUBMIT;
	buf1[1]=listenPort;
	connfd=Open_clientfd(conn_ip, conn_port);
    Rio_writep(connfd,buf1,2*sizeof(int));
	Rio_writep(connfd,program_name,SIZEMAX);
	Rio_writep(connfd,data_name,SIZEMAX);
	Rio_writep(connfd,output,SIZEMAX);
	Rio_writep(connfd,&num,sizeof(int));
    close(connfd);
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
