#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>


#define FILE "shared_memory"

int shmid;

void funcExit(int sig)
{
    printf("\n");
    struct shmid_ds *buf = 0;
    shmctl(shmid, IPC_RMID, buf);

    exit(0);
}

int main()
{
    signal(SIGINT, funcExit);
    char * addr;
    key_t key = ftok(FILE, 'a');
    shmid = (shmget(key, 32, IPC_CREAT | 0666));

    if( shmid == -1 )
    {
        printf("Can't create shared memory\n");
        exit(0);
    }

    if((addr = shmat( shmid, NULL, 0 )) == (char*)-1)
    {
            printf("Shmat err\n");
			exit(0);
	}

    time_t timer = time(0);
    time_t buft = timer;

    if (strlen(addr) != 0)
    {
        printf("there is already a sending process\n");
        exit(0);
    }

    while(1)
    {
        timer = time(0);
        if (timer != buft)
        {
            buft = timer;
            sprintf(addr, "time_Producer = %spid_Producer = %d\n", ctime(&timer), getpid());
        }
    }
    return 0;
}