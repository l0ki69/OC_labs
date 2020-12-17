#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/shm.h>
#include <signal.h>

#define FILE "shared_memory"

pthread_mutex_t mutex;

int shmid;

void funcExit(int sig)
{
    printf("\n");
    struct shmid_ds *buf = 0;
    shmctl(shmid, IPC_RMID, buf);

    exit(0);
}

void* read_func()
{
	key_t key = ftok(FILE, 'a');
	shmid = shmget (key, 32, IPC_EXCL | 0666);

    if (shmid == -1)
    {
    	printf ("Shared memory creation failed\n");
        return 0;
    }

	char* addr = shmat (shmid, NULL, 0);
	if (addr == (char*)-1)
    {
		printf ("Shmat failed\n");
		return 0;
	}

	while (1)
    {
        pthread_mutex_lock (&mutex);
		printf ("%lx %s\n", pthread_self(), addr);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}

	return 0;
}

int main()
{
	//  Thread initialization
    signal(SIGINT, funcExit);

    key_t key = ftok(FILE, 'a');
	int smid = shmget (key, 32, IPC_CREAT | 0666);

	pthread_t pth[10];

	pthread_mutex_init (&mutex, NULL);

    for (int i = 0; i < 10; i++)
    {   
	    pthread_create (&pth[i], NULL, read_func, NULL);
    }

	if (smid == -1)
	{
		printf ("Shared memory creation failed\n");
		return 0;
	}

	char* addr = shmat (smid, NULL, 0);
	if (addr == (char*)-1)
    {
		printf ("Shmat failed\n");
		return 0;
	}

    int count = 0;

	while (1)
    {
        pthread_mutex_lock(&mutex);
       	sprintf(addr, "%d", count);
        pthread_mutex_unlock(&mutex);
		sleep (1);
        count++;
	}

	pthread_mutex_destroy(&mutex);
	return 0;
}