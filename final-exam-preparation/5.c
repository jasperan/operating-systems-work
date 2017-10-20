#include <stdio.h>
#include <pthread.h>
/*
  Exercise 1 final 2013 2013.pdf
*/
int main (int argc, char * argv[])
{
  int fd[2];
  if (argc != 3 || atoi(argv[1]) < 0 || atoi(argv[2]) < 0)
  {
    return -1;
  }
  int num_integers = atoi(argv[1]);
  int auxiliary;
  int array[argc-1];
  for (auxiliary = 0; argv[auxiliary] != NULL; auxiliary++)
  {
    array[auxiliary] = atoi(argv[auxiliary]);
  }
  int search_num = atoi(argv[2]);

  int pipeline[2][2];
  pipe(pipeline[0]);
  pipe(pipeline[1]);

  int search_size = (argc-1)/2;
  int aux;
  for (aux = 0; aux < 2; aux++)
  {
    fd[aux] = fork();
    if (fd[aux] < 0) return -1;
    else if (fd[aux] == 0)
    {
      if (aux == 0)
      {
        int i;
        for (i = 0; i < search_num; i++)
        {
          if (array[i] == search_num)
          {
            printf("Found it.\n");
            write(pipeline[0][1], &search_num, sizeof(search_num));
          }
        }
      }
      else if (aux == 1)
      {
        int i;
        for (i = search_num; i < (argc-1); i++)
        {
          if (array[i] == search_num)
          {
            printf("Found it.\n");
            write(pipeline[0][1], &search_num, sizeof(search_num));
          }
        }
      }
      else return -1;
    }
  }
  int storer1, storer2;
  if (wait(&status1) > 0 && wait(&status2) > 0)
  {
    if (read(pipeline[0][0], &storer1, sizeof(storer1)) < 0)
    {
      return -1;
    }
   if (read(pipeline[1][0], &storer2, sizeof(storer2)) < 0)
   {
     return -1;
   }
   printf("%d\n", storer1);
   printf("%d\n", storer2);
  }
}
