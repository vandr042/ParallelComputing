#include "ss_mpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <limits.h>
#include <time.h>
#include "cv_fileIO.h"
static void print_time(
    double const seconds);
static void print_numbers(char const* const filename, int const * const numbers, size_t const nnumbers);


int main(int argc, char* argv[]){
	srand(time(NULL));
	const char* filename, *outfile,*times = "times.txt";
	int n,myrank,nlocal,npes,nsorted;
	int* buffer, *elmnts, *result, *nsortedBuf;
	FILE* fp;
	struct timeval tval;
	time_t init, final;
	if (argc < 3){
		abort();
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	MPI_Comm_size(MPI_COMM_WORLD,&npes);

	/* read in integers */
	if (myrank == 0){
		filename = argv[1];
		outfile = argv[2];
		n = getLines(filename);
		buffer = (int*) malloc(sizeof(int)*n);
		buffer = readIntsSerial(filename, n);
	}
	MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
	nlocal = n/npes;
	elmnts = (int*) malloc(sizeof(int) * nlocal);
	MPI_Scatter(buffer, nlocal, MPI_INT, elmnts, nlocal, MPI_INT, 0, MPI_COMM_WORLD);
	
	int i,j;
	int *sorted_elmnts,*splitters, *allpicks;
	int *scounts, *sdispls, *rcounts, *rdispls;

	gettimeofday(&tval, NULL);
	init = tval.tv_sec;
	splitters = (int *) malloc(npes * sizeof(int));
	allpicks = (int *) malloc(npes*(npes-1) * sizeof(int));
	/* sort local elements */
	quicksort(elmnts,0,nlocal-1);

	for (i = 1; i < npes; i++){
		splitters[i-1] = elmnts[i*nlocal/npes];
	}
	MPI_Allgather(splitters,npes-1,MPI_INT,allpicks,npes-1,MPI_INT,MPI_COMM_WORLD);

	/* sort sample and choose splitters */
	
	quicksort(allpicks,0,npes*(npes-1));
	for (i=1; i < npes; i++){
		splitters[i-1] = allpicks[i*npes];
	}

	splitters[npes-1] = INT_MAX;
	
	scounts = (int*) malloc(npes*sizeof(int));
	for (i = 0; i < npes; i++){
		scounts[i] = 0;
	}
	
	for (j = i = 0; i < nlocal; i++){
		if (elmnts[i] < splitters[j])
			scounts[j]++;
		else
			scounts[++j]++;
	}
	sdispls = (int *) malloc(npes*sizeof(int));
	sdispls[0] = 0;
	for (i = 1; i < npes; i ++){
		sdispls[i] = sdispls[i-1]+scounts[i-1];
	}

	rcounts = (int *)malloc(npes*sizeof(int));
	MPI_Alltoall(scounts,1,MPI_INT,rcounts,1,MPI_INT,MPI_COMM_WORLD);
	rdispls = (int *)malloc(npes*sizeof(int));
	rdispls[0] = 0;
	for (i = 1; i < npes; i++){
		rdispls[i] = rdispls[i-1] + rcounts[i-1];
	}
	
	nsorted = rdispls[npes-1] + rcounts[i-1];
	sorted_elmnts = (int *) malloc((nsorted)*sizeof(int));
	MPI_Alltoallv(elmnts,scounts,sdispls,MPI_INT,sorted_elmnts,rcounts,rdispls,MPI_INT,MPI_COMM_WORLD);

	/* sort buckets */
	quicksort(sorted_elmnts,0,nsorted-1);
	free(splitters);free(allpicks);free(scounts);free(sdispls);
	free(rcounts);free(rdispls);
	if (myrank == 0){
		result = (int*) malloc(sizeof(int)*n);
		nsortedBuf = (int*) malloc(sizeof(int)*npes);
	}

	/* merge buckets */
	MPI_Gather(&nsorted, 1, MPI_INT, nsortedBuf, 1, MPI_INT, 0, MPI_COMM_WORLD);
	int fdispls[npes];
	if (myrank == 0){
		fdispls[0] = 0;
		for (i = 1; i < npes; i++){
			fdispls[i] = fdispls[i-1] + nsortedBuf[i-1];
		}
	}
	MPI_Gatherv(sorted_elmnts, nsorted, MPI_INT, result, nsortedBuf, fdispls, MPI_INT, 0, MPI_COMM_WORLD); 
	gettimeofday(&tval, NULL);
	final = tval.tv_sec;
	if (myrank == 0){
//		print_numbers(outfile, (int const * const) result, (size_t const) n);
		print_time((double const) (final-init));
	}	
	
	MPI_Finalize();
	return 0;
}




static void print_time(
    double const seconds)
{
  printf("Sort Time: %0.04fs\n", seconds);
}


static void print_numbers(char const* const filename, int const * const numbers, size_t const nnumbers){
	size_t i;
	FILE* fout;
	if ((fout = fopen(filename, "w")) == NULL){
		fprintf(stderr, "error opening %s \n", filename);
		abort();

	}
	for (i = 0; i < nnumbers; i++){
		fprintf(fout, "%d\n", numbers[i]);
	}
	fclose(fout);
}


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


/* need to seed before using rand */
int random_partition(int *a, int p, int r){
	int i = rand() % ((r+1)-p);
	int temp;
	temp = a[p+i];
	a[p+i]=a[r];
	a[r] = temp;
	return partition(a,p,r); 
}

void quicksort(int* a, int p, int r){
	int q;
	
	if (p < r){
		q = random_partition(a,p,r);
		quicksort(a,p,q-1);
		quicksort(a,q+1,r); 
	}
}
