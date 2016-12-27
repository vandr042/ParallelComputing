#include "cv_fileIO.h"

FILE* openFileRead(const char* filename){
	FILE* fp;
	
	if ((fp = fopen(filename,"r")) == NULL){
		fprintf(stderr, "Could not open file.");
		exit(1);
	}	
	return fp;
}

int getLines(const char* filename){
	FILE* fp;	
	char ch;
	int lines;
	
	fp = openFileRead(filename);
	lines = 0;
	
	while(!feof(fp)){
		ch = fgetc(fp);
		if (ch == '\n'){
			lines++;
		}
	}

	fclose(fp);
	return lines;
}	

int* readIntegers(const char* filename, int split, int block, int numP){
	FILE* fp;
	int i,j,newInt;
	int* buffer;
	
	fp = openFileRead(filename);
	
	buffer = (int*) malloc(sizeof(int) * (block*numP));
	
	for (i = j = 0; i < numP; i++, j=0){
		while (j < split){
			if (fscanf(fp, "%d", &newInt) == 0){
				perror("Invalid value in file.");
				abort();
			}
			buffer[block*i+j] = newInt;		
			j++;
		}
		if (i == numP-1){
			while (j < block){
				if (fscanf(fp, "%d", &newInt) == 0){
					perror("Invalid value in file.");
					abort();
				}
				buffer[block*i + j] = newInt;
				j++;
			}
		}
	} 	
	fclose(fp);
	return buffer;
}

int* readIntsSerial(const char* filename, int size){
	FILE* fp;
	int i,newInt;
	int* buffer;
	fp = openFileRead(filename);
	
	buffer = (int*) malloc(sizeof(int) * size);
	
	for (i = 0; i < size; i++){
		if (fscanf(fp, "%d", &newInt) == 0){
			perror("Invalid value in file.");
			abort();
		}
		buffer[i] = newInt;		
	}
	return buffer;
}
