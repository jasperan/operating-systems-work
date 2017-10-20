#include <stdio.h> // sprintf
#include <stdlib.h>
#include <unistd.h> // fork...
#include <fcntl.h>
#include <stddef.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h> // For waiting for the process manager
#include <string.h> // for strtok library function
#include <semaphore.h>
#include <dirent.h>

#define READ_ITERATION 256 // To read every time 256 bytes from the file, for example.
#define MAX_ERROR 1024
#define SIZE_ARG 32 // Size for storing the names of semaphores
int status; // For waiting for the process manager

int get_file_size(int file_descriptor);


int main(int argc, const char *argv[])
{
    char errorprinter[MAX_ERROR];
    if (argc != 2)
    {
        perror("Invalid number of arguments");
        exit(0);
    }
    // It will only receive one argument: the path to a file which will contain the input parameters that will configure the operation of the factory
    int fdreader = open(argv[1], O_RDONLY, 0777);
    if (fdreader < 0)
    {
        perror("[ERROR][factory_manager] Invalid file");
        exit(0);
    }
    int size_file = get_file_size(fdreader); // We invoke the method to get the size of the file.

    int val;
    char readbuffer[size_file+1]; // +1 to hold also the '\0' character.
    bzero(readbuffer, sizeof(readbuffer)); // We zero the array to avoid issues with data garbage.

    val = read(fdreader, readbuffer, size_file); // We read the whole contents of the file.
    if (val < 0)
    {
        perror("[ERROR][factory_manager] Invalid file");
        exit(0);
    }

    // Now we have the information stored in the readbuffer. Let's extract the set of information from it, using the function strtok.
    char * token;
    token = strtok(readbuffer, " ");
    int max_factory_managers = atoi(token);
    if (max_factory_managers < 0)
    {
        perror("Error with the max_factory_managers variable");
        exit(0);
    }

    int id_process_manager[max_factory_managers];
    int size_belt[max_factory_managers];
    int num_elements[max_factory_managers];

    int aux = 0;
    while (token != NULL)
    {
        if ((token = strtok(NULL, " ")) != NULL)
        {
            id_process_manager[aux] = atoi(token);
            if (id_process_manager[aux] < 0)
            {
                perror("Error with the id_process_manager");
                exit(0);
            }
        }
        else break;

        if ((token = strtok(NULL, " ")) != NULL)
        {
            size_belt[aux] = atoi(token);
            if (size_belt[aux] < 0)
            {
                perror("Error with the size_belt");
                exit(0);
            }

        }
        else break;

        if ((token = strtok(NULL, " ")) != NULL)
        {
            num_elements[aux] = atoi(token);
            if (num_elements[aux] < 0)
            {
                perror("Error with the num_elements");
                exit(0);
            }
        }
        else break;

        aux++;
    }

    if (aux > max_factory_managers)
    {
        perror("Passed more things than factory managers are");
        exit(0);
    }

    // Now we have all the information stored, and the input parsed into convenient arrays of values.
    max_factory_managers = aux; // Now the updated value will be the one that we used to read before. If they just passed us three factory managers, even though the maximum was 5, we will just have to create three process managers. Therefore, we update this value to be the one in which 'aux' got stuck before, to avoid these issues.


    sem_t * semaphore[max_factory_managers];
    char sem_name[max_factory_managers][SIZE_ARG];
    int sem_number[max_factory_managers];

    for (aux = 0; aux < max_factory_managers; aux++)
    {
        // CREATING SEMAPHORE AND SENDING POST SIGNAL:  Creation of the synchronization structures necessary for the correct operation of the factory.
        sem_number[aux] = aux;
        sprintf(sem_name[aux], "Semaphore_%d", sem_number[aux]);
        semaphore[aux] = sem_open(sem_name[aux], O_CREAT, 0777, 0);
        if (semaphore[aux] == SEM_FAILED)
        {
            sprintf(errorprinter, "Error opening the semaphore %s\n", sem_name[aux]);
            write(2, &errorprinter, strlen(errorprinter+1));
            exit(0);
        }
    }
    int a;
    // CREATION OF ALL THE PROCESS_MANAGER PROCESSES:
    for (aux = 0; aux < max_factory_managers; aux++)
    {
        a = fork();
        if (a == -1)
        {
            perror("Error creating the process managers' processes");
            exit(0);
        }
        else if (a == 0)
        {
            char args1[SIZE_ARG];
            sprintf(args1, "%d", id_process_manager[aux]);
            char args2[SIZE_ARG];
            sprintf(args2, "Semaphore_%d", sem_number[aux]);
            char args3[SIZE_ARG];
            sprintf(args3, "%d", size_belt[aux]);
            char args4[SIZE_ARG];
            sprintf(args4, "%d", num_elements[aux]);
            // It is assumed that the process manager executable can be found in the current working directory from where the factory manager has been executed.
            char * args[]= {"./process", args1, args2, args3, args4, (char*)0};
            if (execvp("./process", args) < 0)
            {
                sprintf(errorprinter, "[ERROR][factory_manager] Process_manager with id %d has finished with errors.", id_process_manager[aux]);
                write(2, errorprinter, strlen(errorprinter+1));
            }

        }
        else // parent
        {
            printf("[OK][factory_manager] Process_manager with id %d has been created.\n", id_process_manager[aux]);
        }
    }
    sem_t * semaphore_parent = sem_open("Semaphore_Parent", O_CREAT, 0777, 0);
    for (aux = 0; aux < max_factory_managers; aux++)
    {
        sem_post(semaphore[aux]);
        sem_wait(semaphore_parent);
        wait(&status);
    }
    printf("[OK][factory_manager] Finishing.\n");
    return 0;
}

int get_file_size(int file_descriptor)
{
    int returner;
    // WE GET THE SIZE OF THE FILE
    returner = lseek(file_descriptor, 0, SEEK_END);
    if (returner < 0)
    {
        perror("[ERROR][factory_manager] Invalid file");
        exit(0);
    }
    lseek(file_descriptor, 0, SEEK_SET); // Now that we got the size of the file, just reset the pointer to the start of the file again.
    return returner;
}
