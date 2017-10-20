#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define	N 10

pthread_mutex_t mutex;
pthread_attr_t attr;
pthread_cond_t cond1, cond2;

int shared_data = 0;

int turn1=1;
int turn2=0;

void * even(void *arg){
  int i;
  for(i=0;i<N;i++) {

    pthread_mutex_lock(&mutex);
    while(turn1 == 0 ){
      pthread_cond_wait(&cond1, &mutex);
    }
    printf("Thread1= %d\n", shared_data++);
    turn2 = 1;
    turn1 = 0;
    pthread_cond_signal(&cond2);
    pthread_mutex_unlock(&mutex);

  }
  pthread_exit(0);
}

void * odd(void * arg){
  int i;
  for(i=0;i<N;i++) {

    pthread_mutex_lock(&mutex);
    while(turn2 == 0){
      pthread_cond_wait(&cond2, &mutex);
    }
    printf("Thread2= %d\n", shared_data++);
    turn1 = 1;
    turn2 = 0;
    pthread_cond_signal(&cond1);
    pthread_mutex_unlock(&mutex);

  }
  pthread_exit(0);
}

int main(){
  pthread_t th1, th2;

  pthread_attr_init(&attr);
  pthread_mutex_init(&mutex,NULL);
  pthread_cond_init(&cond1,NULL);
  pthread_cond_init(&cond2,NULL);

  pthread_create(&th1, NULL, even, NULL);
  pthread_create(&th2, NULL, odd, NULL);
  pthread_join(th1, NULL);
  pthread_join(th2, NULL);

  pthread_attr_destroy(&attr);
  pthread_mutex_destroy(&mutex);
  pthread_cond_destroy(&cond1);
  pthread_cond_destroy(&cond2);
  exit(0);
}
