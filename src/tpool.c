#include<stdlib.h>
#include <pthread.h>


#include "tpool.h"


/**
 * For status of tasks in the queue
 *
 */
typedef enum {
    PENDING,
    ONGOING,
    DONE
}e_task_status;

/**
 * A struct for thread pool tasks management
 *
 * @var func:       points to the function that needs to be executed
 * @var argument:   arguments that'll be passed onto the function
 * @var status:     an enum type to keep track of current status of the task in queue
 *
 */
typedef struct {
    void (*func)(void *);
    void *argument;
    e_task_status status;
}s_tpool_task;


struct tpool_t {
  pthread_mutex_t lock;
  pthread_cond_t notify;                // notify worker threads
  pthread_t *threads;                   // a list of worker threads
  s_tpool_task *queue;                  // a pointer to the individual tasks queue
  int num_of_threads;                   // total number of thread created inside the thread pool
  int queue_size;                       // size of the queue/ the size of the ring buffer for tasks
  int head;                             // to read items from the buffer
  int tail;                             // to insert data into the buffer
  int active_threads;                   // number of active threads
};
