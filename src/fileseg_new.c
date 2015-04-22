
// e.g. fileSeg("abc",10)
#include <stdio.h>
#include "csapp.h"
#include "string.h"
#include <pthread.h>
#include "stdlib.h"
#include "server.h"
#include "fileseg.h"

int fileSeg(char filename[],int n)
{ 
  char sfilename[20];
	char sfile[20];
	char dfilename[20];
	char ch;
	
	char test_file[20];
	int i=0;


	int idx=0;
	int size;
	int num_lines=0; //number of lines in file
	int lines_per_file=0;// how many lines per file 
	int lines_per_file_end=0;
	int end_flag=0;

	FILE *sfp,*dfp;
    
	strcpy(sfile,filename);
	sprintf(sfilename,"%s",sfile);
	
	num_lines=scanline(sfilename)-1;  
	lines_per_file=num_lines/n;
	lines_per_file_end=num_lines-lines_per_file*(n-1);
	if(num_lines<n){lines_per_file=1;}

	if((sfp=fopen(sfilename,"rb"))==0)
	{
		printf("Can't open the file :%s\n",sfilename);
		exit(0);
	}
  
  	while( idx<n )
	{
		sprintf(dfilename,"%s_%d",sfile,idx);
  		if((dfp=fopen(dfilename,"wb"))==0)
  		{
  		   printf("Can't open or create the file :%s\n",dfilename);
  		   exit(0);
  		}
		 idx=idx+1;
  		////for the last file 
		if (idx==n)
  		{
  			end_flag=1;
		}
		
		////only modified FilePartition
	  	FilePartition(sfp,dfp,lines_per_file,lines_per_file_end,end_flag);//replicate the content from file to file 
	 }

	return 0;
}

// replicate the content from file to file
////only modified FilePartition
int FilePartition(FILE *sfp, FILE *dfp, int size,int size_end,int flag)
{
	char buff1[20];
	char buff2[20];
	char ch;

	int buff_count=0;
	int buff_count1=0;
	
	
	int s=size;
	int s_end=size_end;
	char line[256]=""; 
	if (flag==1)
	{
			while (s_end!=0)
			{
				fgets(line, 256, sfp); 
				printf("%s",line);
				fprintf(dfp,"%s",line);
				s_end=s_end-1;
				if(line[strlen(line)-1]!='\n') {break;}
				
			}
	}
	else
	{
		while ( (s!=0))
		{	

			
			fgets(line, 256, sfp);
			fprintf(dfp,"%s",line);
			s=s-1;
		
		}
	
	///for the key
	
	//get the last string of segemented file
	buff_count=strlen(line)-strlen(strstr(line,"|||"));
	strncpy(buff1,line,buff_count);
	buff1[buff_count]='\0';
	
	//get the next string 
	fgets(line, 256, sfp);
	buff_count1=strlen(line);
	buff_count=strlen(line)-strlen(strstr(line,"|||"));
	strncpy(buff2,line,buff_count);
	buff2[buff_count]='\0';
	
	
	//compare
	while (!strcmp(buff1,buff2))
	{
		fprintf(dfp,"%s",line);
    fgets(line, 256, sfp);
		buff_count1=strlen(line);
		buff_count=strlen(line)-strlen(strstr(line,"|||"));
		strncpy(buff2,line,buff_count);
		buff2[buff_count]='\0';
	}
  
    fseek(sfp,-buff_count1,SEEK_CUR);
	//fgets pointer previous line
	}
	fclose(dfp);
	return 0;
}

////count the number of lines of file
int scanline(char * filename)
{
    FILE * f = 0; 
	char line[256]=""; 
	int lines = 0;
    	f = fopen(filename, "r");
    	if(!f) return 0;
    	while(!feof(f)) 
	{ 
		fgets(line, 256, f); 
		lines++;
	}
    fclose(f);
    return lines;
}
