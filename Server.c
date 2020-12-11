#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define path "/home/yulya/kt/FIFO.fifo"
#define part_path "/home/yulya/kt/"
#define N 50

int main(int argc, char * argv[]){

	if ( mkfifo (path, 0600) == -1){
		if (errno != EEXIST){
			printf("create fifo error\n");
			exit (errno);
		}
	}

	int fd_r, fd_w, fd;
	char *data;
	data = (char *) malloc (N*sizeof(char));
	char *file;
	file = (char *) malloc (N*sizeof(char));

	struct stat *about_file = (struct stat *) malloc (sizeof(struct stat));

	while (1){

		fd_r = open (path, O_RDONLY); //блокируется, пока фифо не откроется для записи
		if ( fd_r == -1 ){
			printf("open fifo1 error\n");
			exit (errno);
		}
		
		if( read(fd_r, data, N)  == -1){
			printf("read from fifo2 error\n");
			exit (errno);
		}
		close(fd_r);
		
		file = strchr (data, ';');
		*file = '\0';
		file++;
		char new_path[N];
		sprintf(new_path, "%sclient%s.fifo",part_path, data);

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

		if ( stat(file, about_file) == -1){
			printf("stat error\n");
			exit(errno);
		}
		long int file_size = about_file->st_size;
		char * buf = (char *) malloc (file_size * sizeof (char));
		if( read(fd, buf, file_size)  == -1){
			printf("read from file error\n");
			exit (errno);
		}
		close(fd);
		fd_w = open(new_path, O_WRONLY);
		sleep(10);
		if ( write(fd_w, buf, file_size) == -1){
			printf("write in fifo error\n");
			exit (errno);
		}
		close(fd_w);
		free(buf);
	
	}
	free(data);
	free(file);
	free(about_file);
}

