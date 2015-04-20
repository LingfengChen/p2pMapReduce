#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fileSeg(char filename[],int n);//file segmentation (e.g. fileSeg("abc",10)) 
void FilePartition(FILE *sfp, FILE *dfp, int size,int flag); //replicate the content from file to file 
int scanline(char * filename);//number of lines the file has 
