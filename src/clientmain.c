#include <stdio.h>
#include "csapp.h"
#include "string.h"
#include <pthread.h>
#include "stdlib.h"
#include "client.h"
int main(int argc, char *argv[])
{
    int listenPort;
    listenPort = atoi(argv[1]);
    pthread_t tid;
    int* args;
    args=(int*)Malloc(sizeof(int));
    *args=listenPort;
    Pthread_create(&tid,NULL,cmd,(void*)args);
    pthread_detach(tid);
    if(argc==2){
        Listening(listenPort);
    }
    else
    {
      printf("Wrong Arg\n");
    }
  	return 0;
}