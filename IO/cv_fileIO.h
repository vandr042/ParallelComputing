#ifndef CV_FILEIO_H
#define CV_FILEIO_H

#include <stdlib.h>
#include <stdio.h>

int getLines(const char* fp);
int* readIntegers(const char* fp, int split, int block, int numP);
FILE* openFileRead(const char* filename);
int* readIntsSerial(const char* fp, int size);
#endif

