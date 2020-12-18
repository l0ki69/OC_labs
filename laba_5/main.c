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

pthread_mutex_t mutex;

int count = 0;

void funcExit(int sig)
{
    printf("\n");
    exit(0);
}

void* read_func(int num)
{
	while (1)
    {
        pthread_mutex_lock (&mutex);
		printf ("Я %d-й поток, мой tid =  %lx, счетчик  = %d\n", num , pthread_self(), count);
		pthread_mutex_unlock(&mutex);
		sleep(1);
	}

	return 0;
}

void* write_func()
{
	while (1)
	{   
		pthread_mutex_unlock(&mutex);
		count++;
		printf("Я увеличил счетчик до значения %d\n", count);
		sleep(rand() % 10);
		pthread_mutex_lock(&mutex);
		if (count > 10000) count = 0;
	}
}

int main()
{
	//  Thread initialization
    signal(SIGINT, funcExit);

	pthread_t pth[11];

	pthread_mutex_init (&mutex, NULL);

	fsync(&stdout);

    for (int i = 0; i < 10; i++)
    {   
		int num = i + 1;
	    pthread_create (&pth[i], NULL, read_func, num);
    }

	pthread_create(&pth[10], NULL, write_func, NULL);

	for (int i = 0; i < 11; i++)
	{
		pthread_join(&pth[i], NULL);
	}


	pthread_mutex_destroy(&mutex);
	return 0;
}