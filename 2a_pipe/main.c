#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main()
{
	int fd[2];
	if(pipe(fd) < 0){
		printf("Failed pipe open\n");
		exit(0);
	}

	time_t ttime;

	pid_t pid = fork();
	if(pid == 0)  //CHILD - source
    {	
	    //sleep(1);
		printf("CHILD: get sys time from pipe\n");

		close(fd[1]);
		long int Mas[2] = {0, 0};
		read(fd[0], Mas, sizeof(long int) * 2);
		time_t timer = time(0);
		printf("CHILD:\nchild_time = %s", ctime(&timer));
		timer = Mas[0];
		printf("parent_time = %s", ctime(&timer));
		printf("parent_pid = %ld\n", Mas[1]);

		exit(0);
	}
    else  //PARENT - target
    {
		printf("PARENT: put sys time in pipe\n");

		close(fd[0]);
		long int mas[2];
		mas[0] = time(0);
		mas[1] = getpid();
		sleep(5);
		write(fd[1], mas , sizeof(long int) * 2);
		close(fd[1]);

		printf("PARENT: process terminated\n");
	}

	return 0;
}