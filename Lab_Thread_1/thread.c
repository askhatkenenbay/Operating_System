#include <pthread.h> 
#include <stdio.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/time.h>
 
int result[2][2] = {
	{0,0},
	{0,0}
};
int first[2][3] = {
	{1,2,3},
	{4,5,6}
};
int second[3][2] = {
	{1,2},
	{3,4},
	{5,6}
};
pthread_t threads[2];

void sequential(int first[2][3], int second[3][2]); 
void threaded(int first[2][3], int second[3][2]);
long int getCurTime(); 
void toStringResult();
void zeroMatrix(int matrix[2][2]);
void *multiplyRow(void *arg);

int main( int argc, char **argv ) {
	printf("start time: %ld\n",getCurTime());
	sequential(first, second);
	printf("end time: %ld\n",getCurTime());
	toStringResult();
	zeroMatrix(result);
	printf("threaded version begins below\n");
	printf("start time threaded: %ld\n", getCurTime());
	threaded(first,second);
	for(int i=0; i<2;i++){
		pthread_join(threads[i],NULL);
	}
	printf("end time threaded: %ld\n", getCurTime());
	toStringResult();
	pthread_exit( 0 );
	return 0;
}
void toStringResult(){
	for(int i = 0; i < 2; i++) {
		for(int j = 0; j < 2; j++){
			printf("%d ",result[i][j]);
		}
		printf("\n");
	}
}
long int getCurTime(){
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (long int) tv.tv_usec;
}
void zeroMatrix(int matrix[2][2]){
	matrix[0][0]=0;
	matrix[0][1]=0;
	matrix[1][0]=0;
	matrix[1][1]=0;
}
void sequential(int first[2][3], int second[3][2]){
	for(int i = 0; i<2; i++){
		for(int j =0; j<2; j++){
			int res = 0;
			for(int k=0; k<3; k++){
				res += first[i][k] * second[k][j];
			}
			result[i][j] = res;
		}
	}
}
void threaded(int first[2][3], int second[3][2]){
	for(int i = 0; i < 2; i++){
		int *arg = malloc(sizeof(*arg));
		*arg = i;
		pthread_create(&threads[i], NULL, multiplyRow, arg); 
	}
}
void *multiplyRow(void *arg){
	int *rowNum  =(int *) arg;
	for(int j=0;j<2;j++){
		int res = 0;
		for(int k=0;k<3;k++){
			res+=first[*rowNum][k]*second[k][j];
		}
		result[*rowNum][j]=res;
	}
	free(arg);
	pthread_exit( NULL );
}

