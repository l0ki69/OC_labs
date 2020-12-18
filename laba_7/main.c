#include <fcntl.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define READ_THREAD_COUNT 10
pthread_mutex_t mutex;

pthread_cond_t condition;

int count = 0;

void funcExit(int sig) 
{
  printf("\n");
  exit(0);
}

void *read_func(void *num) 
{
	while (1) 
	{
		pthread_mutex_lock(&mutex);
		pthread_cond_wait(&condition, &mutex);
		printf("Я %d-й поток, мой tid =  %lx, счетчик  = %d\n", *(int *)num,
			(long)pthread_self(), count);
		fflush(stdout);
		pthread_mutex_unlock(&mutex);
		sleep(1);
  	}
}

void *write_func(void *arg) 
{
	while (1) 
	{
		sleep(rand() % 10);
		pthread_mutex_lock(&mutex);
		count++;
		if (count > 10000)
		count = 0;
		printf("Я увеличил счетчик до значения %d\n", count);
		printf("...and then relax...\n");
		if (rand() % 2 == 0)
		{	
			printf("Сработал broadcast\n");
			pthread_cond_broadcast(&condition);
		}
		else	
		{	
			printf("Сработал signal\n");
			pthread_cond_signal(&condition);
		}
		fflush(stdout);
		sleep(rand() % 10);
		pthread_mutex_unlock(&mutex);
  	}
}

int main() 
{
	signal(SIGINT, funcExit);

	pthread_t pth[READ_THREAD_COUNT + 1];
	int thread_nums[READ_THREAD_COUNT];
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&condition, NULL);

	for (int i = 0; i < READ_THREAD_COUNT; i++) 
	{
		thread_nums[i] = i;
		pthread_create(&pth[i], NULL, read_func, &thread_nums[i]);
	}

	pthread_create(&pth[READ_THREAD_COUNT], NULL, write_func, NULL);

	for (int i = 0; i < READ_THREAD_COUNT + 1; i++) 
	{
		pthread_join(pth[i], NULL);
	}

	pthread_mutex_destroy(&mutex);
	return 0;
}
