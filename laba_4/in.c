#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <signal.h>

#define FILE "Makefile"

void funcExit(int sig)
{
    printf("\n");
    exit(0);
}


struct sembuf sem_lock = {0,-1,0}, sem_open = {0,1,0};

int main()
{
    signal(SIGINT, funcExit);
    char * addr;
    key_t semkey = ftok("/tmp", 'a');
    int shmid = (shmget(2002, 32, 0666));
    int semid = (semget(semkey, 1, 0666));

    if(semid == -1)
    {
        printf("Sem open err\n");
        exit(0);
    }

    if( shmid == -1 )
    {
        printf("Can't open shared memory\n");
        exit(0);
    }

    if((addr = shmat( shmid, NULL, 0 )) == (char*)-1)
    {
        printf("Shmat err\n");
        exit(0);
    }



    while(1)
    {
        semop(semid, &sem_lock, 1);
        time_t timer = time(0);
        printf("\ntime_Consumer = %spid_Consumer = %d\n", ctime(&timer), getpid());
        printf("\n%s", addr);
        semop(semid, &sem_open, 1);
		sleep(0.5);
    }
    return 0;
}