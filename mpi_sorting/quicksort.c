#include "quicksort.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/* 
 * partions the array based based on the last element of the array
 * and returns the index of this element after partitioning.
 */
int partition(int* a, int p, int r){
	int x, j, i, temp;

	x = a[r];
	i = p-1;
	for (j = p; j <= r-1; j++){
		if (a[j] < x){
			i = i + 1;
			temp = a[i];
			a[i] = a[j];
			a[j] = temp;
		}
	}
	i = i + 1;	
	a[r] = a[i];
	
	a[i] = x;
	return i;	
}

/* partitions the input array based on a random element */
int random_partition(int *a, int p, int r){
	int i = rand() % ((r+1)-p);
	int temp;
	temp = a[p+i];
	a[p+i]=a[r];
	a[r] = temp;
	return partition(a,p,r); 
}

/* sort the input array using the quicksort algorithm */
void quicksort(int* a, int p, int r){
	int q;
	
	if (p < r){
		q = random_partition(a,p,r);
		quicksort(a,p,q-1);
		quicksort(a,q+1,r); 
	}
}

