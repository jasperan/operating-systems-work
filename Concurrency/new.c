#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"
#include <semaphore.h>
#include <string.h>

#define size 64
#define NUM_THREADS 2

pthread_t thid[NUM_THREADS]; // Thread IDs, both for the producer and the consumer.
pthread_attr_t attr; // For the attributes of the threads.
pthread_mutex_t mutex;

struct producer_args
{
int example;
};

void producer(struct producer_args * args)
{

	struct producer_args producer;

	memcpy (&producer, args, sizeof(struct producer_args));
	
	printf("Thread %d\n", producer.example);
}

int main(int argc, char * argv[])
{
	char example[size];
	strcpy(example, argv[1]);
	struct producer_args * producer_args;


        producer_args = (struct producer_args * )malloc(sizeof(struct producer_args));
	producer_args->example = atoi(example);

	pthread_attr_init(&attr);
	pthread_create(&thid[1], &attr, (void*)producer, producer_args);

	pthread_join(thid[1], NULL);

	free(producer_args);

	return 0;
}
