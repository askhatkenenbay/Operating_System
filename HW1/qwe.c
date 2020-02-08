#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>


int replace(int fd, off_t offset, char *s, int n);

int prog(int sourcefile, int destfile, off_t offset, int length);
int insert_at(int fd, off_t offset, char *buf, int n);

int main(int argc, char *argv[]){
	if(argc != 5){
		perror("Not enough arguments");
		exit( -1);
	}
	int fileOpen = open(argv[1], O_RDWR);
	if(fileOpen == -1){
		perror("File does not exist or can't be opened");
		close(fileOpen);
		return -1;
	}
	int destfile = open(argv[2], O_RDWR);
	if(destfile == -1){
		destfile = creat(argv[2], S_IRWXU );
	}
	printf("destfile: %d\n",destfile);
	int temp = prog(fileOpen, destfile, atoi(argv[3]), atoi(argv[4]) ); 
	if(temp != -1){
		printf("EveryThing fine\n");
	}
	close(fileOpen);
	close(destfile);
	return 0;
}

int insert_at(int fd, off_t offset, char *buf, int n){
	if(n>strlen(buf) || n<1 || offset<0){
		return -1;
	}
	int size =  lseek(fd, 0, SEEK_END);
	lseek(fd, offset, SEEK_SET);
	int temp = size - offset;
	printf("temp: %d\n",temp);
	char *c = (char *) calloc((temp), sizeof(char)); 
	int readbytes = read(fd, c, temp);
	printf("readbytes: %d\n",readbytes);
	if( readbytes == 0 ){
		lseek(fd, offset, SEEK_SET);
		int writenBytes = write(fd,buf,n);
		if(writenBytes == -1){
			return -1;
		}
		return offset+writenBytes;
	} else {
		int newOffset = replace(fd,offset,buf, n );
		printf("newOffset: %d\n",newOffset);
		return replace(fd, newOffset, c, readbytes);
	}
	return readbytes;
}
int replace(int fd, off_t offset, char *s, int n){
	/*if( strlen(s) + offset > lseek(fd, 0, SEEK_END) ){
		printf("Exceeds limit!");
		return -1;
	}*/
	if(strlen(s)>n){
		return -1;
	}
	if(n<1){
		return -1;
	}
	
	if(offset<0){
		return -1;
	}
	lseek(fd, offset, SEEK_SET);
	int writenBytes = write(fd,s,n);
	if(writenBytes == -1){
		return -1;
	}
	return offset+writenBytes;
}
//if error returns -1 else
int prog(int sourcefile, int destfile, off_t offset, int length){
	if( length + offset > lseek(sourcefile, 0, SEEK_END) ){
		printf("Exceeds limit!");
		return -1;
	}
	lseek(sourcefile, offset, SEEK_SET);
	char *c = (char *) calloc((length), sizeof(char)); 
	int readbytes = read(sourcefile, c, length);
	if(readbytes != length){
		printf("Problem with reading");
		return -1;
	}
	printf("readbytes: %d\n",readbytes);
	int temp = insert_at(destfile,offset,c,length);
	if(temp==-1){
		printf("Error, cannot replace");
		return -1;
	}
	free(c);
	return temp;
}
