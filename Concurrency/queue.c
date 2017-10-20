#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "queue.h"
#include <string.h> // strlen function
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h> // For write

int TOTAL_ITEMS; // Will hold the total number of items to be inserted in the queue.

#define MAX_ERROR 1024

struct queue
{
    struct element * list; // A list of elements
    int valid_items; // Number of valid items in the queue at any given point time.
    int last_pos; // Last used position, either for insertion or deletion of an element from the queue.
};
struct queue * newQ; // We declare the pointer to the queue here, otherwise the methods wouldn't be able to use it, if this structure weren't passed as parameter. As the assignment specifies the set of parameters for each function, we declare this pointer as global.

// ASKED METHODS: A detailed explanation can also be found in the report for each one of the methods.

// Returns 0 on success
int queue_init(int size)
{
    if (size < 1) // Check that the size is valid. In case it isn't, show the error and exit.
    {
        char errorprinter[MAX_ERROR];
        sprintf(errorprinter, "[ERROR][queue] There was an error while using queue with id: %d.", newQ->list[0].id_belt);
        write(2, errorprinter, strlen(errorprinter+1));
        return -1;
    }
    // We reserve memory dynamically
    newQ = (struct queue *)malloc(sizeof(struct queue));
    newQ->list = (struct element *) malloc(sizeof(struct element)*size);

    // And we assign default values to the queue (both for the integer values, and for the list of elements, for each element):
    newQ->valid_items = 0;
    newQ->last_pos = 0;
    TOTAL_ITEMS = size;
    int aux;
    for (aux = 0; aux < size; aux++)
    {
        newQ->list[aux].num_edition = 0;
        newQ->list[aux].id_belt = 0;
        newQ->list[aux].last = 0;
    }
    return 0;
}

// Returns 0 on success
int queue_destroy(void)
{
    // Basically, it frees the resources that were previously dynamically allocated.
    free(newQ->list);
    free(newQ);
    return 0;
}

// Enqueuing an element: Returns 0 on success
int queue_put(struct element * elem)
{
    // We get our auxiliary struct to hold the values, get some variables to hold temporally the values passed, and put the element in the queue. Finally, we print an informative message.
    struct element element;
    memcpy(&element, elem, sizeof(struct element));
    int num_edition_temporal = element.num_edition;
    int id_belt_temporal = element.id_belt;
    int last_temporal = element.last;

    newQ->valid_items++;
    newQ->list[newQ->last_pos].num_edition = num_edition_temporal;
    newQ->list[newQ->last_pos].id_belt = id_belt_temporal;
    newQ->list[newQ->last_pos].last = last_temporal;
    newQ->last_pos = newQ->last_pos+1 % TOTAL_ITEMS;
    /*
    This way, I will never get a segmentation fault or anything due to running out of
    available positions. I will never go to the TOTAL_ITEMS+1 position, and this should be
    the correct implementation of the circular queue, since the TOTAL_ITEMS+1'th element will be
    assigned to the first array position again, in case the space is not occupied again.
    */
    printf("[OK][queue] Introduced element with id: %d in belt %d.\n", num_edition_temporal, id_belt_temporal);
    return 0;
}

// Dequeuing an element:
struct element * queue_get (void)
{
    // The process is the opposite to the queue_put. We decrement the variables, and retrieve the element from the queue (return it.) We finally print an informative message, and return the appropriate element from the list in the queue.
    newQ->valid_items--;
    int position = newQ->last_pos;
    int num_edition_temporal = newQ->list[position].num_edition;
    int id_belt_temporal = newQ->list[position].id_belt;
    newQ->last_pos = newQ->last_pos-1 % TOTAL_ITEMS;
    /*
    Here, we will also never run out of range due to the circular implementation.
    In case we are trying to go into the last_pos, we will go to the position -1. But,
    -1 mod TOTAL_ITEMS is equal to -1+TOTAL_ITEMS mod TOTAL_ITEMS. Circular and elegant.
    */
    printf("[OK][queue] Obtained element with id: %d in belt %d.\n", num_edition_temporal, id_belt_temporal);
    return &newQ->list[position];
}

// Returns 1 if the queue is empty, 0 otherwise.
int queue_empty(void)
{
    if (newQ->valid_items == 0)
    {
        return 1; // If empty.
    }
    else
    {
        return 0; // If not empty.
    }
}

// Returns 1 if the queue is full, 0 otherwise.
int queue_full(void)
{
    if (newQ->valid_items == TOTAL_ITEMS)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
