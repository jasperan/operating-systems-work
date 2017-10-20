#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
/*
  Exercise 3 ssoo_final_2013_2013.pdf
*/
#define NUM_PLAYER_THREADS 100
#define NUM_WORKER_THREADS 5

void* playerfunct(void);
void* workerfunct(void);

pthread_attr_t attr;
pthread_mutex_t player, worker;
pthread_cond_t playercond, workercond;
pthread_t thid_players[NUM_PLAYER_THREADS], thid_workers[NUM_WORKER_THREADS];
int occupied = 0, player_occupied = 0;

int main (int argc, char * argv[])
{
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(PTHREAD_CREATE_JOINABLE);
  pthread_cond_init(&playercond, NULL);
  pthread_cond_init(&workercond, NULL);
  pthread_mutex_init(&player, NULL);
  pthread_mutex_init(&worker, NULL);

  int aux;
  for (aux = 0; aux < NUM_PLAYER_THREADS; aux++)
  {
    pthread_create(thid_players[aux], &attr, playerfunct, NULL);
  }
  for (aux = 0; aux < NUM_WORKER_THREADS; aux++)
  {
    pthread_create(thid_workers[aux], &attr, workerfunct, NULL);
  }

  for (aux = 0; aux < NUM_PLAYER_THREADS; aux++)
  {
    pthread_join(thid_players[aux], NULL);
  }
  for (aux = 0; aux < NUM_WORKER_THREADS; aux++)
  {
    pthread_join(thid_workers[aux], NULL);
  }

  pthread_mutex_destroy(&worker);
  pthread_mutex_destroy(&player);
  pthread_cond_destroy(&playercond);
  pthread_cond_destroy(&workercond);
  pthread_attr_destroy(&attr);


}

void* playerfunct(void)
{
  usleep(100);
  while (occupied)
  {
    pthread_cond_wait(&playercond, &player);
  }
  pthread_cond_signal(&playercond);
  player_occupied++;
  // Print player number on screen
  printf("My player number is %u\n", (unsigned int)pthread_self());
  sleep(rand()%6);
  player_occupied--;
  pthread_exit(0);
}
void* workerfunct(void)
{
  pthread_mutex_lock(&worker);
  while (player_occupied > 0)
  {
    pthread_cond_wait(&workercond, &worker);
  }
  pthread_cond_signal(&workercond);
  occupied = 1;

  printf("My worker number is %u\n", (unsigned int)pthread_self());
  sleep(rand()%4);
  printf("My work is done\n");
  pthread_mutex_unlock(&worker);
  pthread_exit(0);
}
