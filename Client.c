#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define page_size 4096
#define N 256

int main(int argc, char * argv[]){

	if (argc != 2){
		printf("arguments error\n");
		exit(-1);
	}

	char dir[N];
	if (getcwd(dir, N) == NULL) {
		printf("getcwd error\n");
		exit(errno);
	}
	char path[N];
	
	sprintf(path, "%s/FIFO.fifo", dir);

	char *data = (char *) malloc (N*sizeof(char));
	char *file = (char *) malloc (N*sizeof(char));
	char * text = (char *) malloc (page_size * sizeof (char));
	int fd_r, fd_w;

	fd_w = open (path, O_WRONLY);
	if ( fd_w == -1 ){
		printf("open fifo1 error\n");
		exit (errno);
	}

	pid_t id;
	id = getpid();
	sprintf(data, "%d;%s", id, argv[1]);
	if ( write(fd_w, data, N) == -1 ){
		printf("write in fifo error\n");
		exit (errno);
	}
	close(fd_w);

	char new_path[N];
	sprintf(new_path, "%s/client%d.fifo", dir, id);

	if ( mkfifo (new_path, 0600) == -1){
		if (errno != EEXIST){
			printf("create fifo error\n");
			exit (errno);
		}
	}

	fd_r = open (new_path, O_RDONLY);
	if ( fd_r == -1 ){
		printf("open fifo2 error\n");
		exit (errno);
	}

	while(1){
		int rd =  read(fd_r, text, page_size);
		if (rd == -1){
			printf("write in fifo error\n");
			exit (errno);
		}
		
		if ((write(1, text, rd)) == -1) {
			printf("write error\n");
			exit(errno);
		}
		if (rd < page_size)
			break;
	}
	close(fd_r);
	free(data);
	free(file);
	free(text);
}

