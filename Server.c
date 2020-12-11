#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define page_size 4096
#define N 50

void *p[4];
char dir[N];

void my_handler(int sig){
	free(p[0]);
	free(p[1]);
	free(p[2]);
	free(p[3]);
	sprintf(dir, "%s/FIFO.fifo", dir);
	if (remove (dir) == -1){
		printf("remove error");
		exit(errno);
		}	
	exit(0);
	
}


int main(int argc, char * argv[]){

	signal(SIGINT, my_handler);
	
	if (getcwd(dir, N) == NULL) {
		printf("getcwd error\n");
			exit(errno);
	}
	char path[N];
	
	sprintf(path, "%s/FIFO.fifo", dir);

	if ( mkfifo (path, 0600) == -1){
		if (errno != EEXIST){
			printf("create fifo error\n");
			exit (errno);
		}
	}

	int fd_ser, fd_cl, fd;
	char *data = (char *) malloc (N*sizeof(char));
	p[0] = (void *) data; 
	char *file = (char *) malloc (N*sizeof(char));
	p[1] = (void *) file; 
	struct stat *about_file = (struct stat *) malloc (sizeof(struct stat));
	p[2] = (void *) about_file; 
	char * buf;
	buf = (char *) malloc (page_size * sizeof (char));
	if (buf == NULL){
		printf("buf malloc error\n");
		exit(errno);
	}
	p[3] = (void *) buf;
	fd_ser = open (path, O_RDWR);
		if ( fd_ser == -1 ){
			printf("open fifo1 error\n");
			exit (errno);
		}

	while (1){

		if( read(fd_ser, data, N)  == -1){
			printf("%s\n", data);
			printf("read from fifo error\n");
			exit (errno);
		}	

		file = strchr (data, ';');
		*file = '\0';
		file++;
		char new_path[N];
		sprintf(new_path, "%s/client%s.fifo",dir, data);

		fd = open(file, O_RDONLY);
		if ( fd == -1 ){
			printf("open file error\n");
			exit (errno);
		}
		if ( mkfifo (new_path, 0600) == -1){
			if (errno != EEXIST){
				printf("create fifo error\n");
				exit (errno);
			}
		}
	
		if ( fd_ser == -1 ){
			printf("open fifo error\n");
			exit (errno);
		}

		if ( stat(file, about_file) == -1){
			printf("stat error\n");
			exit(errno);
		}
		long int file_size = about_file->st_size;
		int i;
		
		fd_cl = open(new_path, O_WRONLY);		
		for ( i = 0; i < file_size / page_size; i++ ){

			int rd = read(fd, buf, page_size);
			if( rd  == -1){
				printf("read from file error\n");
				exit (errno);
			}	
			int wr = write(fd_cl, buf, page_size);
			if (wr == -1){
				printf("write in fifo error\n");
				exit (errno);
			}			
		}

		int rd = read(fd, buf, file_size%page_size);
			if( rd  == -1){
				printf("read from file error\n");
				exit (errno);
			}
		int wr = write(fd_cl, buf, file_size%page_size);
			if (wr == -1){
				printf("write in fifo error\n");
				exit (errno);
			}

		if (remove (new_path) == -1){
			printf("remove error");
			exit(errno);
		}		
		
	
	}
}

