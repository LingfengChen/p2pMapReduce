#ifndef __FILESEG_H__
#define __FILESEG_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int fileSeg(char filename[],int n);//file segmentation (e.g. fileSeg("abc",10)) 
int FilePartition(FILE *sfp, FILE *dfp, int size,int size_end,int flag); //replicate the content from file to file 
int scanline(char * filename);//number of lines the file has 

#endif /* __FILESEG_H__ */