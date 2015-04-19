#include <stdio.h>
#include "csapp.h"
#include "string.h"
#include <pthread.h>
#include "stdlib.h"
#include "server.h"

int main(int argc, char *argv[])
{
  	int listenPort;
  	pthread_t tid;
  	Pthread_create(&tid,NULL,cmd,NULL);
  	pthread_detach(tid);
  	int i=0;
  	for(i=0;i<TABLESIZE;i++)
  	{
  		  Table[i].status=EMPTY;
  	}
  	if(argc==2){
      	listenPort = atoi(argv[1]);
  		  Listening(listenPort);
  	}
  	else
  	{
  		printf("Wrong Arg\n");
  	}
  	return 0;
}