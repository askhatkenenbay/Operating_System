#include <pthread.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/time.h>
 
pthread_t threads[1024];
int globalFirst[1024][1024];
int globalSecond[1024][1024];
int globalResult[1024][1024];
int globalCols = 0; 
int globalRows = 0;
void sequential(); 
void threaded(); 
long int getCurTime(); 
void toStringResult(int i); 
void *multiplyRow(void *arg);
void zeroMatrix(); 
int main( int argc, char **argv ) {
	if(argc != 5){
		fprintf(stderr, "can't use\n");
		exit(-1);
	}
	globalCols = atoi(argv[2]);
	globalRows = atoi(argv[1]);
	if(globalRows >1024 || globalRows<1 || globalCols>1024 || globalCols<1){
		fprintf(stderr, "problem with rows or cols or both");
		exit(-1);
	}
	int val1 = atoi(argv[3]);
	int val2 = atoi(argv[4]);
	for(int i=0;i<globalRows;i++){
		for(int j=0;j<globalCols;j++){
			globalFirst[i][j] = val1;
			globalSecond[j][i] = val2;
		}
	}
	struct timeval start;
	struct timeval end;
	gettimeofday(&start,NULL);
	fprintf(stderr,"start time seq: %ld\n",start.tv_sec*100000+start.tv_usec);
	sequential();
	gettimeofday(&end,NULL);
	fprintf(stderr, "end time: %ld\ndifference:%ld\n",end.tv_sec * 1000000 + end.tv_usec, end.tv_sec * 1000000 + end.tv_usec - (start.tv_sec * 1000000 + start.tv_usec));
//	toStringResult(1);
	zeroMatrix();
	gettimeofday(&start,NULL);
	fprintf(stderr,"start time threaded %ld\n", start.tv_sec*1000000+start.tv_usec);
	threaded();
	for(int i=0; i<globalRows;i++){
		pthread_join(threads[i],NULL);
	}	
	gettimeofday(&end,NULL);
	fprintf(stderr, "end time threaded %ld\ndifference:%ld\n",end.tv_sec*1000000+end.tv_usec, end.tv_sec*1000000+end.tv_usec -(start.tv_sec*1000000+start.tv_usec));
	toStringResult(0);	
	pthread_exit( 0 );
	return 0;
}
void zeroMatrix(){
	for(int i=0;i<globalRows;i++){
		for(int j=0;j<globalCols;j++){
			globalResult[i][j]=0;
		}
	}
}
void toStringResult(int is_seq){
	for(int i = 0; i < globalRows; i++) {
		for(int j = 0; j < globalRows; j++){
			if(is_seq != 0){
				fprintf(stderr, "%d ", globalResult[i][j]);
			}else{
				printf(" %d ", globalResult[i][j]);
			}
		}
		if(is_seq != 0){
			fprintf(stderr,"\n");
		}else{
			printf("\n");
		}
	}
}
long int getCurTime(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (long int) tv.tv_usec;
}
void sequential(){
	for(int i = 0; i<globalRows; i++){
		for(int j =0; j<globalRows; j++){
			int res = 0;
			for(int k=0; k<globalCols; k++){
				res += globalFirst[i][k] * globalSecond[k][j];
			}
			globalResult[i][j] = res;
		}
	}
}
void threaded(){
	for(int i = 0; i < globalRows; i++){
		int *arg = malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&threads[i], NULL, multiplyRow,arg);
	}
}
void *multiplyRow(void *arg){
	int *rowNum =(int *) arg;
	for(int j=0;j<globalRows;j++){
		int res = 0;
		for(int k=0;k<globalCols;k++){
			res += globalFirst[*rowNum][k] * globalSecond[k][j];
		}
		globalResult[*rowNum][j]=res;
	}
	free(arg);
	pthread_exit( NULL );
}
