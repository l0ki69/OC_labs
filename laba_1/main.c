#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<unistd.h>

void funcExit(){
	printf("atexit in proccess with pid = %d\n",getpid());
	return;
}

int main()
{
    pid_t pid = fork();
    atexit(funcExit);

    if (pid == 0) //CHILD
    {
        printf("CHILD:\nchild_pid = %d\nchild_ppid = %d\n", getpid(), getppid());
        exit(0);
    }
    else //PARENT
    {
        wait(NULL);
    }

    printf("PARENT:\nparent_pid = %d\nparent_ppid = %d\n", getpid(), getppid());

    return 0;
}