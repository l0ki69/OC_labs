#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>


int main () {
    pid_t pid;
    char buff[100];
	int rv;
	
	memset(buff, '\0', 100);
	unlink("fifo");
	if((mkfifo("fifo", 0777)) == -1){
		printf("Can't create FIFO");
		exit(0);
	}
	
    pid = fork();
    if (pid == 0){	//CHILD - source
		printf("CHILD: hello\n");
        int fd = open("fifo", O_WRONLY);
		if(fd == -1){
			printf("CHILD: Can't open FIFO");
			exit(0);
		}
		
		time_t data = time(0);
		sprintf(buff, "%s", ctime(&data));
        write(fd, buff, 100);
        close(fd);
		
		printf("CHILD: process terminated\n");
		exit(1);
    }else{	//PARENT - target
		printf("PARENT: hello\n");
        int fd = open("fifo", O_RDONLY);
		if(fd == -1){
			printf("PARENT: Can't open FIFO");
			exit(0);
		}
		
        read(fd, buff, 100);
		close(fd);
		
        printf("PARENT: %s\n", buff);
		printf("PARENT: process terminated\n");
    }
}
