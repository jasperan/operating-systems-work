#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#define NUM_THREADS 3
int overall_coins;
pthread_t thid[NUM_THREADS];
pthread_attr_t attr;
pthread_cond_t cond1;
pthread_mutex_t mutex;
int finish = 0;

int main(int argc, char ** argv)
{
  int aux;
  pthread_attr_init(&attr);
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&cond, NULL);
  pthread_attr_setdetachstate(PTHREAD_CREATE_JOINABLE);
  for (aux = 0; aux < NUM_THREADS; aux++)
  {
    pthread_create(&thid[aux], &attr, thread_function, aux+1);
  }


  for (aux = 0; aux < NUM_THREADS; aux++)
  {
    pthread_join(&thid[aux], NULL);
  }
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond);
}

void thread_function(int idthread)
{
  int local_coins = rand() % 6;
  int pred_total_coin = local_coins + (rand() % 11);

  pthread_mutex_lock(&mutex);
  int estimation = overall_coins + local_coins;
  finish++;
  while (finish != 3)
  {
    pthread_cond_wait(&cond, &mutex);
  }
  pthread_mutex_unlock(&mutex);

  if (estimation == overall_coins)
  {
    printf("%d has won the game.\n", idthread);
  }
}
