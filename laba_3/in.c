#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

#define FILE "shared_memory"

void funcExit(int sig)
{
    printf("\n");
    exit(0);
}

int main()
{
    signal(SIGINT, funcExit);

	char * addr;
	int mem;
    key_t key = ftok(FILE, 'a');
	if( (mem = (shmget(key, 32, IPC_EXCL))) == -1 )
    {
        printf("SM open eror\n");
        exit(0);
    }

    if((addr = shmat( mem, NULL, 0 )) == (char*)-1)
    {
		printf("SM link error\n");
        exit(0);
    }

    while(1)
    {   
        time_t timer = time(0);
        printf("\ntime_Consumer = %spid_Consumer = %d\n", ctime(&timer), getpid());
        printf("\n%s", addr);
        sleep(1);
    }
    return 0;
}