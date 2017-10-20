#include <stdio.h>
#include <string.h>

/*
  Exercise 4 ssoo_final_2013_2014.pdf
*/
int replace (char * file_in, char * file_out, char character_old, char character_new);
int main(int argc, char **argv)
{
  if (argc != 5)
  {
    return(-1);
  }
  if (argv[1] == NULL || argv[2] == NULL || argv[3] == NULL || argv[4] == NULL)
  {
    exit(-1);
  }
  if (replace(argv[1], argv[2], argv[3], argv[4]) < 0)
  {
    perror("Error on the function replace");
    return(-1);
  }

  return 0;
}

int replace (char * file_in, char * file_out, char character_old, char character_new)
{
  char buffer[2];
  bzero(buffer, sizeof(buffer));

  int file_size = lseek(fd, 0, SEEK_END);
  if (lseek(fd, 0, SEEK_SET) < 0)
  {
    exit(-1);
  }
  char final_buffer[file_size+1];
  int fd = open(file_in, O_RDWR);
  if (fd < 0)
  {
    exit(-1);
  }
  int val;
  while (val < file_size)
  {
    if (read(fd, buffer, 1) < 0) exit(-1);
    if (strcmp(buffer, &character_old) == 0)
    {
      write(final_buffer, &character_new, 1);
    }
    write(final_buffer, buffer, 1);
    val++;
  }
}
