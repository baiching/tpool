#include<stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>


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
  pthread_t *worker_threads;                   // a list of worker threads
  s_tpool_task *queue;                  // a pointer to the individual tasks queue
  int num_of_threads;                   // total number of thread created inside the thread pool
  int queue_size;                       // size of the queue/ the size of the ring buffer for tasks
  int head;                             // to read items from the buffer
  int tail;                             // to insert data into the buffer
  int active_threads;                   // number of active threads
};

/**
 * A worker thread
 *
 * @param pool:                         ref to the owner of this thread
 *
 */
static void *f_worker_thread(void *pool);


static int *f_tpool_free(void *pool);

static tpool_t *init(tpool *pool){
    pool = (tpool_t *)malloc(sizeof(tpool_t));

    pool->head = 0;
    pool->tail = 0;
    pool->active_threads = 0;

    return pool;
}

tpool_t *f_tpool_create(int num_of_threads, int queue_size){
    // Threalpool creation goes here
    tpool_t *pool = NULL;

    if(num_of_threads <= 0 || num_of_threads > MAX_THREADS || queue_size<=0 || queue_size > MAX_QUEUE){
        printf("Invalid num_of_threads or queue size.\n");
        return NULL;
    }

    if(init(&pool) == NULL){
        printf("Threadpool memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    pool->num_of_threads = num_of_threads;

    pool->worker_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_of_threads);
    pool->queue = (s_tpool_task *)malloc(sizeof(s_tpool_task) * queue_size);

    if(pthread_mutex_init(&(pool->lock), NULL) < 0){
        printf("Error occured: %d.\n", errno);
        free(pool);
        exit(EXIT_FAILURE);
    }

    // Initilizing notifying variable
    if(pthread_cond_init(&(pool->notify), NULL) < 0){
        printf("Error occured: %d.\n", errno);
        free(pool);
        exit(EXIT_FAILURE);
    }


}











