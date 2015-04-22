#include <stdio.h>
#include "csapp.h"
#include "string.h"
#include <pthread.h>
#include "stdlib.h"
#include "server.h"
#include "fileseg.h"

void* cmd()
{
	char input[SIZEMAX];
	char ips[SIZEMAX];
	int i=0;
	struct clientNode Task[TABLESIZE];
	int numActual;
    while(1)
    {
      	fgets(input,SIZEMAX,stdin);
      	input[strlen(input)-1]='\0';
      	if(!strcmp(input,"checktable"))
      	{
      		printf("___________________________\n");
      		printf("These users are online:\n");
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
      	else if(!strcmp(input,"checkselect"))
      	{
      		selectNode(5,Task,&numActual);
      		printf("___________________________\n");
      		printf("These %d users are chosen for the task:\n",numActual);
      		for(i=0;i<numActual;i++)
      		{
      			ip_convert(Task[i].addr,ips);
      			printf("%s:%d\n",ips,ntohs(Task[i].addr.sin_port));
      		}
      		printf("___________________________\n");
      	}
      	else if(!strcmp(input,"checkduplicate"))
      	{
      		fileDuplicate("a",5);
      	}
      	else if(!strcmp(input,"checkseg"))
      	{
      		fileSeg("a",5);
      	}
      	else if(!strcmp(input,"checkmerge"))
      	{
      		fileMerge(3,"a");
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

void* handle(void *Args)
{
	SA clientaddr;
	int clientfd;
	int ret;
	int i=0;
	clientaddr=((struct para*)Args)->addr;
	clientfd=((struct para*)Args)->fd;
	free(Args);
	int buf1[SIZEMAX];
	int buf2[SIZEMAX];
	char cmd[SIZEMAX];
	char file_name[SIZEMAX];
	char program_name[SIZEMAX];
	char data_name[SIZEMAX];
	char output[SIZEMAX];
	int num=0;
	int numActual=0;
	struct clientNode Task[TABLESIZE];
	int tasknum=0;
	Rio_readp(clientfd,buf1,2*sizeof(int));
	printf("New Requst %d\n",buf1[0]);
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
	else if(buf1[0]==DOWNLOAD)
	{
		printf("Download request\n");
		Rio_readp(clientfd,file_name,SIZEMAX);
		ret=fileSend(clientfd,file_name);
	}
	else if(buf1[0]==UPLOAD)
	{
		printf("Upload request\n");
		Rio_readp(clientfd,file_name,SIZEMAX);
		ret=fileRecv(clientfd,file_name);
	}
	else if(buf1[0]==SUBMIT)
	{
		printf("Submit request\n");
		Rio_readp(clientfd,program_name,SIZEMAX);
		Rio_readp(clientfd,data_name,SIZEMAX);
		Rio_readp(clientfd,output,SIZEMAX);
		Rio_readp(clientfd,&num,sizeof(int));
		printf("%s %s %s %d\n",program_name,data_name,output,num);
		selectNode(num,Task,&numActual);
		printf("actual num node is %d\n",numActual);
		fileSeg(data_name,numActual);
		for(i=0;i<numActual;i++)
		{
			if(allocateTask(i,Task[i],program_name,data_name,output)==-1)
			{
				while(1)
				{
					printf("Task %d Need spare wheel\n",i);
					ret=findSpareWheel(i,program_name,data_name,output);
					if(ret!=-1)
					{
						printf("Spare Wheel over\n");
						break;
					}
				}
			}
			printf("allocation %d is over\n",i);
		}
		fileMerge(numActual,output);
		for(i=0;i<numActual;i++)
		{	
			sprintf(cmd,"rm %s_%d",output,i);
			system(cmd);
		}
		for(i=0;i<numActual;i++)
		{	
			sprintf(cmd,"rm %s_%d",data_name,i);
			system(cmd);
		}
	}
	close(clientfd);
}

int tableAdd(SA addr)
{
	pthread_t tid;
	int i=0;
	int* Args;
	for(i=0;i<TABLESIZE;i++)
	{
		if(Table[i].status==EMPTY)
		{
			Args=(int*)Malloc(sizeof(int));
			*Args=i;
        	Pthread_create(&tid,NULL,keepAlive,Args);
        	pthread_detach(tid);
        	Table[i].addr=addr;
			Table[i].status=ONLINE;
			Table[i].tid=tid;
			Table[i].index=i;
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

void selectNode(int num,struct clientNode Task[SIZEMAX],int* numActual)
{
	int i=0;
	int size=0;
	for(i=0;i<TABLESIZE;i++)
	{
		if(Table[i].status==ONLINE)
		{
			Task[size]=Table[i];
			Table[i].status=BUSY;
			size++;
			printf("find node for the task\n");
			if(size>=num)
				break;
		}
	}
	*numActual=size;
}
int findSpareWheel(int taskid,char* program_name,char* data_name,char* output)
{
	int i=0;
	struct clientNode sw;
	for(i=0;i<TABLESIZE;i++)
	{
		if(Table[i].status==ONLINE)
		{
			//need mutex actually
			sw=Table[i];
			Table[i].status=BUSY;
			allocateTask(taskid,sw,program_name,data_name,output);
			break;
		}
	}
}
int allocateTask(int taskid,struct clientNode client,char* program_name,char* data_name,char* output)
{
	int connfd;
	int rc;
	int buf1[SIZEMAX];
	char prog[SIZEMAX];
	char data[SIZEMAX];
	char outp[SIZEMAX];
	char taskid_s[SIZEMAX];
	sprintf(taskid_s,"%d",taskid);
	strcpy(prog,program_name);
	strcpy(data,data_name);
	strcpy(outp,output);
	strcat(data,"_");
	strcat(data,taskid_s);
	strcat(outp,"_");
	strcat(outp,taskid_s);
	if ((connfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return -1;
	if (connect(connfd, (struct sockaddr *) &(client.addr), sizeof(SA)) < 0)
	{
		Table[client.index].status=OFFLINE;
		return -1;
	}
	buf1[0]=ALLOCATE;
	buf1[1]=taskid;
	rc=Rio_writep(connfd,buf1,2*sizeof(int));
	if(rc<=0)
	{
		Table[client.index].status=OFFLINE;
		return -1;
	}
	rc=Rio_writep(connfd,prog,SIZEMAX);
	if(rc<=0)
	{
		Table[client.index].status=OFFLINE;
		return -1;
	}
	rc=Rio_writep(connfd,data,SIZEMAX);
	if(rc<=0)
	{
		Table[client.index].status=OFFLINE;
		return -1;
	}
	rc=Rio_writep(connfd,outp,SIZEMAX);
	if(rc<=0)
	{
		Table[client.index].status=OFFLINE;
		return -1;
	}
	rc=fileSend(connfd,prog);
	if(rc<=0)
	{
		Table[client.index].status=OFFLINE;
		return -1;
	}
	rc=fileSend(connfd,data);
	if(rc<=0)
	{
		Table[client.index].status=OFFLINE;
		return -1;
	}
	rc=fileRecv(connfd,outp);
	if(rc<=0)
	{
		Table[client.index].status=OFFLINE;
		return -1;
	}
	Table[client.index].status=ONLINE;
	close(connfd);
	return 0;
}

void fileDuplicate(char* file_name,int num)
{
	int i=0;
	int rc=0;
	FILE* fp;
	FILE* fp1;
	char buf1[FILEMAX];
	char file[SIZEMAX];
	char id[SIZEMAX];
	fp=fopen(file_name,"r");
	rc=fread(buf1,sizeof(char),FILEMAX,fp);
	for(i=0;i<num;i++)
	{
		sprintf(id,"_%d",i);
		strcpy(file,file_name);
		strcat(file,id);
		fp1=fopen(file,"w");
		fwrite(buf1,sizeof(char),rc,fp1);
		fclose(fp1);
	}
	fclose(fp);
}

void fileMerge(int num,char* output)
{
	int i=0;
	FILE* fp[SIZEMAX];
	FILE* fpo;
	fpo=fopen(output,"w");
	char file[SIZEMAX];
	int top[SIZEMAX];
	char topline[SIZEMAX][SIZEMAX];
	char minline[SIZEMAX];
	int min=-1;
	for(i=0;i<num;i++)
	{
		sprintf(file,"%s_%d",output,i);
		fp[i]=fopen(file,"r");
	}
	for(i=0;i<num;i++)
	{
		top[i]=0;
		fgets(topline[i],SIZEMAX,fp[i]);
	}
	while(1)
	{
		min=-1;
		for(i=0;i<num;i++)
		{
			if(top[i]!=-1)
			{
				if(min==-1)
				{
					strcpy(minline,topline[i]);
					min=i;
					continue;
				}
				if(strcmp(topline[i],minline)<0)
				{
					min=i;
					strcpy(minline,topline[i]);
				}
			}
		}
		if(min==-1)
			break;
		printf("%s\n",minline);
		fputs(minline,fpo);
		if(fgets(topline[min],SIZEMAX,fp[min])==NULL)
		{
			top[min]=-1;
		}
		else
		{
			top[min]++;
		}
	}
	for(i=0;i<num;i++)
	{
		fclose(fp[i]);
	}
	fclose(fpo);
}

void *keepAlive(void *Args)
{
	int tid=*(int*)Args;
	SA clientaddr=Table[tid].addr;
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
			Table[tid].status=EMPTY;
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
			Table[tid].status=OFFLINE;
			close(fd);
			continue;
		}
		if(fd<=0)
		{
			printf("fd < 0 retry %d\n",retry);
			retry++;
			Table[tid].status=OFFLINE;
			sleep(5);
			close(fd);
			continue;
		}

		num=Rio_writen(fd,(char*)buf1,2*sizeof(int));
		if(num<=0)
		{
			printf("can not write retry %d\n",retry);
			retry++;
			Table[tid].status=OFFLINE;
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
			Table[tid].status=OFFLINE;
			sleep(5);
			close(fd);
			continue;
		}
		close(fd);
		retry=0;
		Table[tid].status=ONLINE;
	}
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
