#include <stdio.h>
#include <string.h>
#include <pthread.h>

pthread_t thid[2];
pthread_attr_t attr;
pthread_cond_t finished0, finished1;
pthread_mutex_t mutex;

void thread0(int * array);
void thread1(int * array);
int main(int argc, char ** argv)
{
  int array[100];
  bzero(array, sizeof(array));
  int aux, aux2;
  for (aux = 0; aux < 200; aux++)
  {
    for (aux2 = 0; aux2 < 100)
    {
      array[aux] = (array[aux] + array[(aux+1)%100] + array[(aux-1) %100]) / 3;
    }
  }

  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
  pthread_mutex_init(&mutex, NULL);
  pthread_cond_init(&finished0, NULL);
  pthread_cond_init(&finished1, NULL);


  pthread_create(&thid[0], &attr, thread0, array);
  pthread_create(&thid[1], &attr, thread1, array);
  pthread_join(&thid[0], NULL);
  pthread_join(&thid[1], NULL);

}

void thread0(int * array)
{
  int aux[100/2];
  bzero(aux, sizeof(aux));
  for (auxiliary = 0; auxiliary < 200; auxiliary++)
  {
    for (aux2 = 0; aux2 < 50)
    {
      aux[aux2] = (aux[aux2] + aux[(aux2+1)%100] + aux[(aux2-1) %100]) / 3;
    }
  }
}
void thread1(int * array)
{
  int aux[100/2];
  bzero(aux, sizeof(aux));
  for (auxiliary = 0; auxiliary < 200; auxiliary++)
  {
    for (aux2 = 50; aux2 < 100)
    {
      aux[aux2] = (aux[aux2] + aux[(aux2+1)%100] + aux[(aux2-1) %100]) / 3;
    }
  }
}
