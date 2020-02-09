#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define CMDLEN 254
#define MAX_ARG 101

int main()
{
	int pid;
	int status;
	int i, fd;
	char command[CMDLEN];
	char* arrayStr[MAX_ARG];
	int is_back; //0 false, 1 true
	int arr_one; //0 false, 1 true
	int arr_two; //0 false, 1 true
	printf( "Program begins.\n" );
	for (;;)
	{
		printf( "smsh %%:   " );
		is_back = 0;
		fgets( command, CMDLEN, stdin );
		command[strlen(command)-1] = '\0';
		if ( strcmp(command, "quit") == 0 )
			break;
		if(command[strlen(command) - 1] == '&'){
			is_back = 1;
			command[strlen(command) - 1] = '\0';
		}
		i = 0;
		arrayStr[i] = strtok(command, " ");
		while(arrayStr[i] != NULL){
			i++;
			arrayStr[i] = strtok(NULL," ");
			if(i>MAX_ARG-1){
				printf("Error i>100");
				exit(-1);
			}
		}
		arr_one =0;
		arr_two =0;
		if(i>1){
			if(strcmp(arrayStr[i-2],">>") == 0){
				arr_two = 1;
				arrayStr[i-2] = NULL;
			}else if(strcmp(arrayStr[i-2],">") == 0){
				arr_one = 1;
				arrayStr[i-2] = NULL;
			}
		}else{
			arrayStr[i]=NULL;
		}
		pid = fork();
		if ( pid < 0 )
		{
			printf( "Error in fork.\n" );
			exit(-1);
		}
		if ( pid != 0 )
		{
			printf( "PARENT. pid = %d, mypid = %d.\n", pid, getpid() );
			if(is_back == 0){
				waitpid( pid, &status, 0 );
			}
			if(status==65280){
				printf("No such command: %s\n",arrayStr[0]);
				exit(-1);
			}
		}
		else
		{
			printf( "CHILD. pid = %d, mypid = %d.\n", pid, getpid() );
			int fd;
			if(arr_one == 1){
				fd = open( arrayStr[i-1], O_WRONLY|O_CREAT|O_TRUNC,0777);
				dup2(fd,1);
				close(fd);
			}else if(arr_two == 1){
				fd = open( arrayStr[i-1], O_WRONLY|O_CREAT,0777);
				lseek(fd,0,SEEK_END);
				dup2(fd,1);
				close(fd);
			}
			int err = execvp(arrayStr[0],arrayStr);
			if(err==-1){
				exit(-1);
			}
			break;
		}
	}
}
