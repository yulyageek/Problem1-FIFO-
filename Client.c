#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#define path "/home/yulya/kt/FIFO.fifo"
#define part_path "/home/yulya/kt/"
#define max_file_size 4096

int main(int argc, char * argv[]){
	if (argc != 2){
		printf("arguments error\n");
		exit(-1);
	}

	struct stat *about_file = (struct stat *) malloc (sizeof(struct stat));
	char *data;
	data = (char *) malloc (50*sizeof(char));
	char *file;
	file = (char *) malloc (50*sizeof(char));
	char * text;
	text = (char *) malloc (max_file_size * sizeof (char));
	int fd_r, fd_w;

	fd_w = open (path, O_WRONLY|O_NONBLOCK);
	if ( fd_w == -1 ){
		printf("open fifo error\n");
		exit (errno);
	}
	pid_t id;
	id = getpid();
	sprintf(data, "%d;%s", id, argv[1]);
	if ( write(fd_w, data, 50) == -1 ){
		printf("write in fifo error\n");
		exit (errno);
	}
	close(fd_w);
	char new_path[50];
	sprintf(new_path, "%s/client%d.fifo", part_path, id);

	if ( mkfifo (new_path, 0600) == -1){
		if (errno != EEXIST){
			printf("create fifo error\n");
			exit (errno);
		}
	}

	fd_r = open (new_path, O_RDONLY);
	if ( fd_r == -1 ){
		printf("open fifo error\n");
		exit (errno);
	}

	if( read(fd_r, text, max_file_size)  == -1){
		printf("read from fifo error\n");
		exit (errno);
	}
	close(fd_r);
	printf("%s", text);
}
