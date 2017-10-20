#include <stdio.h>
#include <string.h>
#include <pthread.h>

int status;

int main(int argc, char ** argv)
{
  // Assume the number of children is passed in argv.
  if (argc != 2) return -1;
  if (atoi(argv[1]) < 0) return -1;
  int N = argv[1];
  int i;
  int pid [N];
  int pipe[N][2];
  for (i = 0; i < N; i++)
  {
    pipe(pipe[i]);
  }

  for (i = 0; i < N; i++)
  {
    if (pid[i] == 0)
    {
      if (i == 0)
      {
        close(pipe[i][1]);
        close(pipe[i+1][0]);

        read(pipe[i][0], )
      }
      else if (i == N)
      {
        close(0);
        dup(pipe[i-1][0]);
        close(pipe[i-1][0]);
      }
      else
      {
      close(1);
      dup(pipe[i-1][1]);
      close(pipe[i-1][1]);

      close(0);
      dup(pipe[i+1][0]);
      close(pipe[i+1][0]);
    }
      // They are now connected.
    }
    else
    {
      char buffer [5];
      bzero(buffer, sizeof(buffer));

      write(pipe[i], buffer, sizeof(buffer));
      wait(&status);
    }
  }
}
