#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <pthread.h>
#include "queue.h"
#include <semaphore.h>
#include <string.h>

#define NUM_THREADS 2 // create our pool of threads, one for the producer and one for the consumer.
#define MAX_ERROR 1024 // The maximum length for printing the error message, I have declared as 1024 bytes. But it could be less, actually.
#define SEMAPHORE_SIZE 32 // To store the size of the semaphore names

char errorprinter[MAX_ERROR];


// In process_manager, we receive the parameters from the execvp used by the factory_manager. Now, these parameters become our argv array.
struct producer_args
{
    struct element elem; // An element
    int size_belt; // The size of the belt, in order to print it.
    int num_elements; // The number of elements, in order to print it too.
};

struct consumer_args
{
    int num_elements, id_belt; // Also needed in the consumer function in order to print the corresponding message.
};

pthread_t thid[NUM_THREADS]; // Thread IDs, both for the producer and the consumer. Only two thread ids needed.
pthread_attr_t attr; // For the attributes of the threads.
pthread_mutex_t mutex;


int queueIsEmpty;
int queueIsFull;
// For the conditional messages
pthread_cond_t condproducer;
pthread_cond_t condconsumer;


// PROTOTYPES FOR FUNCTIONS
void producer_function (struct producer_args * args);
void consumer_function (struct consumer_args * args);



int main (int argc, const char * argv[] )
{

    // The information that is received by the factory manager is retrieved in this section. Note that we use integer values to store the passed arguments, so we take advantage of the atoi function.
    int id_process_manager = atoi(argv[1]); // We first get the id of the process manager
    if (id_process_manager < 0)
    {
        perror("[ERROR][process_manager] Arguments not valid.");
        exit(-1);
    }

    char sem_name[SEMAPHORE_SIZE];
    strcpy(sem_name, argv[2]); // Secondly, the passed name of the semaphore by the factory manager.

    int size_belt = atoi(argv[3]); // Third, the size of the belt (MAXIMUM)
    if (size_belt < 0)
    {
        perror("[ERROR][process_manager] Arguments not valid.");
        exit(-1);
    }
    int num_elements = atoi(argv[4]); // And fourth, the needed number of elements that need to be introduced in the queue/belt.
    if (num_elements < 0)
    {
        perror("[ERROR][process_manager] Arguments not valid.");
        exit(-1);
    }
    // Now we have received the information from the execvp successfully.
    printf("[OK][process_manager] Process_manager with id: %d waiting to produce %d elements.\n", id_process_manager, num_elements); // We print the requested message


    // WAIT FOR THE FACTORY MANAGER'S SIGNAL TO START THE EXECUTION:

    sem_t * semaphore1 = sem_open(sem_name, 0); // We open the already created semaphore. In case of error, print it.
    if (semaphore1 < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }
    // And wait for the signal from the factory manager.
    if (sem_wait(semaphore1))
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }

    // Now, we initialize the mutex, and also the conditional variables that are used in our thread functions, to protect the data.
    if (pthread_mutex_init(&mutex, NULL) != 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }
    if (pthread_cond_init(&condproducer, NULL) < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }

    if (pthread_cond_init(&condconsumer, NULL) < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }

    // NOW, TIME TO PASS ARGUMENTS TO A THREAD, AND CREATE IT.
    struct producer_args * producer_args = (struct producer_args *)malloc(sizeof(struct producer_args)); // This pointer will be the one passed as argument to the producer function.

    // We copy the necessary information
    producer_args->size_belt = size_belt; // Size of the circular buffer.
    producer_args->elem.id_belt = id_process_manager;
    producer_args->elem.last = 0;
    producer_args->elem.num_edition = 0;
    producer_args->num_elements = num_elements; // Amount of elements that must be produced in the belt.


    // WE INITIALIZE THE THREAD ATTRIBUTES (even though the JOINABLE is the default attribute, I like to do it anyways.)
    pthread_attr_init(&attr); // Set thread properties, and of type detachable.
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


    // And we create producer's lightweight process, passing its respective set of arguments.
    if (pthread_create(&thid[0], &attr, (void*)producer_function, producer_args) < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }

    // And we do the same with the consumer, pass the arguments and create the thread.
    struct consumer_args * consumer_args = (struct consumer_args*)malloc(sizeof(struct consumer_args));
    consumer_args->num_elements = num_elements;
    consumer_args->id_belt = id_process_manager;

    if (pthread_create(&thid[1], &attr, (void*)consumer_function, consumer_args) < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }

    // WHEN BOTH THREADS ARE DONE, WE JOIN THEM (WAITING FOR THEIR FINALIZATION):
    if (pthread_join(thid[0], NULL) < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }
    if (pthread_join(thid[1], NULL) < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }


    //We also create the second -static- semaphore for signalling the factory manager that the execution of the process manager is done.
    sem_t * semaphore2 = sem_open("Semaphore_Parent", 0);
    if (semaphore2 < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }
    // We send the signal to the factory manager.
    if (sem_post(semaphore2) < 0)
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", id_process_manager);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }

    // We free resources for the arguments to the threads, destroy the queue, destroy the mutex, and the conditional variables also.
    free(producer_args);
    free(consumer_args);
    queue_destroy();
    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condproducer);
    pthread_cond_destroy(&condconsumer);

    return 0;
}


void producer_function (struct producer_args * args) // Function that shall be executed by the producer thread.
{
    struct producer_args holder;

    memcpy(&holder, args, sizeof(struct producer_args)); // We copy the passed arguments into an auxiliary producer_args struct, and use this one instead.
    /*
    Can be used to test that the parameters have been passed correctly to the thread.
    printf("Id belt %d\n", holder.elem.id_belt);
    printf("Last %d\n", holder.elem.last);
    printf("Num edition %d\n", holder.elem.num_edition);
    printf("Size belt %d\n", holder.size_belt);
    printf("Num elements%d\n", holder.num_elements);
    */

    // Initialize the belt's queue
    if (queue_init(holder.size_belt) == -1) // In case that we queue_init was attempted to be joined with a negative size, we show an error and stop execution of this process manager.
    {
        sprintf(errorprinter, "[ERROR][process_manager] There was an error executing process_manager with id %d.\n", holder.elem.id_belt);
        write(2, errorprinter, strlen(errorprinter+1));
        exit(-1);
    }
    printf("[OK][process_manager] Belt with id: %d has been created with a maximum of %d elements.\n", holder.elem.id_belt, holder.size_belt); // In any ther case, we indicate the correct initialization of the queue/belt.


    // Now, we proceed to insert all the elements. In case that the queue is full (meaning that there are not more slots to insert an element at that given point in time, we use conditional variables to indicate that it is not possible at that moment to insert an element, and wait until signalled again)
    int aux;
    for (aux = 0; aux < holder.num_elements; aux++)
    {
        pthread_mutex_lock(&mutex); // We protect the insertion
        while (queueIsFull == 1)
        {
            pthread_cond_wait(&condproducer, &mutex);
        }
        if (aux == holder.num_elements-1) holder.elem.last = 1; // In case that it's the last iteration, we insert an element with last = 1.
        queue_put(&holder.elem); // We put the element into the queue
        queueIsEmpty = 0; // Since we have just inserted the element, we change this variable to indicate that the queue is not empty anymore, and signal the conditional variable in case the consumer was blocked.
        pthread_cond_signal(&condconsumer);
        holder.elem.num_edition++; // We increment the id of the element, in order for the next element to be inserted with a different id.
        pthread_mutex_unlock(&mutex);
    }

    printf("[OK][process_manager] Process_manager with id: %d has produced %d elements.\n", holder.elem.id_belt, holder.num_elements);
    pthread_exit(0); // We finalize the execution of the thread.
}

void consumer_function (struct consumer_args * args)
{
    usleep(100); // We do this to prevent the consumer to be executed before the producer
    struct consumer_args holder2; // Again, we use this temporal struct to store the argument pointer passed as parameter.
    memcpy(&holder2, args, sizeof(struct consumer_args));

    int aux;
    for (aux = 0; aux < holder2.num_elements; aux++) // For as many elements as there have been introduced, we remove the elements from the queue.
    {
        pthread_mutex_lock(&mutex);
        while (queueIsEmpty) // In case the queue is empty, then we wait until we can remove more items.
        {
            pthread_cond_wait(&condconsumer, &mutex);
        }
        queue_get(); // We extract the item, indicate that the queue is not full anymore, and finally signal the producer in order for the producer to be able to insert more items (in case it was blocked due to the full queue.)
        queueIsFull = 0;
        pthread_cond_signal(&condproducer);
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0); // We finalize the execution of the thread.
}
