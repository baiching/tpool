#include<stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>

#include "tpool.h"


/**
 * For status of tasks in the queue
 *
 */
typedef enum {
    TASK_PENDING,
    TASK_ONGOING,
    TASK_FINISHED
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
    uint32_t task_id;
}s_tpool_task;

/**
 * @brief It's exclusively for serving executed tasks taskid on completion
 *
 * @var task_id: the unique identifier for each task
 * @var status: current state of the task
 * @var queue_size: the size of the ring buffer for tasks
 * @var queue_counter: the number of tasks in the queue
 *
 */
struct TaskOut {
    uint32_t task_id;
    e_task_status status;
};

/**
 * @var tlock: lock for output ring
 * @var tnotify: notify worker threads
 * @var out_queue: a pointer to the individual tasks completion queue
 * @var queue_size: size of the output buffer
 * @var queue_counter: active elements on the buffer
 */

typedef struct {
    pthread_mutex_t tlock;
    pthread_cond_t tnotify;
    TaskOut *out_queue;
    int queue_size;
    int queue_counter;
    int head;
    int tail;
}TaskMaintainer;

struct tpool_t {
  pthread_mutex_t lock;
  pthread_cond_t notify;                // notify worker threads
  pthread_t *worker_threads;            // a list of worker threads
  s_tpool_task *queue;                  // a pointer to the individual tasks queue
  int num_of_threads;                   // total number of thread created inside the thread pool
  int queue_size;                       // size of the queue/ the size of the ring buffer for tasks
  int head;                             // to read items from the buffer
  int tail;                             // to insert data into the buffer
  int queue_counter;                    // pending tasks
  int starting_threads;                 // number of threads just started
  int stop;                             // set it as 1 to stop the treads

};

// tid for generating task id
static uint32_t tid = 0;
// this lock is for task id creation only
pthread_mutex_t tlock = PTHREAD_MUTEX_INITIALIZER;
//static struct s_task_out *tout = NULL;

static TaskMaintainer *tout = NULL;



/**
 * A worker thread
 *
 * @param pool:                         ref to the owner of this thread
 *
 */
static void *f_worker_thread(void *tpool);


static int f_tpool_free(tpool_t *pool);

static tpool_t *init(tpool_t *pool){
    pool = (tpool_t *)malloc(sizeof(tpool_t));

    pool->head = 0;
    pool->tail = 0;
    pool->starting_threads = 0;
    pool->queue_counter = 0;
    pool->stop = 0;

    return pool;
}

uint32_t f_get_taskid(void){
    pthread_mutex_lock(&tlock);
    tid++;
    pthread_mutex_unlock(&tlock);

    return tid;
}

tpool_t *f_tpool_create(int num_of_threads, int queue_size){
    // Threalpool creation goes here
    tpool_t *pool = NULL;

    if(num_of_threads <= 0 || num_of_threads > MAX_THREADS || queue_size<=0 || queue_size > MAX_QUEUE){
        printf("Invalid num_of_threads or queue size.\n");
        return NULL;
    }

    if(init(pool) == NULL){
        printf("Threadpool memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    pool->num_of_threads = num_of_threads;

    pool->worker_threads = (pthread_t *)malloc(sizeof(pthread_t) * num_of_threads);
    pool->queue = (s_tpool_task *)malloc(sizeof(s_tpool_task) * queue_size);

    // Initializing completion buffer
    tout = (TaskMaintainer *)malloc(sizeof(TaskMaintainer));
    tout->head = tout->tail = tout->queue_counter = 0;
    tout->queue_size = queue_size;
    tout->out_queue = (TaskOut *)malloc(sizeof(TaskOut) * queue_size);

    //Initializing mutex
    if((pthread_mutex_init(&(pool->lock), NULL) < 0) || (pthread_mutex_init(&(tout->tlock), NULL) < 0)){
        printf("Error occured Initializing mutex: %s.\n", strerror(errno));
        free(pool);
        return NULL;
    }

    // Initilizing notifying variable
    if((pthread_cond_init(&(pool->notify), NULL) < 0) || (pthread_cond_init(&(tout->tnotify), NULL) < 0)){
        printf("Error occured initializing condtionals: %s.\n", strerror(errno));
        free(pool);
        return NULL;
    }

    // creating worker threads
    for(int i = 0; i <num_of_threads; i++){
        // pthread_create takes 4 parameters
        // 1. a pthread_t pointer to store the thread identifier
        // 2. thread attributes (or NULL for defaults)
        // 3. pointer to the function the thread will execute
        // 4. the argument for the worker thread
        if(pthread_create(&(pool->worker_threads[i]), NULL, f_worker_thread, (void *)pool) < 0){
            pool->stop = 1;
            f_tpool_destroy(pool);
            printf("Thread creation has failed: %s", strerror(errno));
            return NULL;
        }

        pool->starting_threads++;
    }


    return pool;
}

int f_tpool_add_task(tpool_t *pool, uint32_t taskid, void (*function)(void *), void *arg){
    int err = 0;

    // checking for empty entries
    if(pool == NULL || function == NULL){
        printf("Threadpool is empty or function wasn't passed.\n");
        return -1;
    }

    // aquiring the lock to ensure none other thread interferes during adding task
    if(pthread_mutex_lock(&(pool->lock)) < 0) {
        printf("Mutex lock failed.\n");
        return -1;
    }

    int next = (pool->tail + 1) % pool->queue_size;

    do {
        // checking if the queue is full or not
        if(pool->queue_counter == pool->queue_size){
            printf("The queue is full.\n");
            err = -1;
            break;
        }

        // adding the task to the queue
        pool->queue[pool->tail].func = function;
        pool->queue[pool->tail].argument = arg;
        pool->queue[pool->tail].task_id = taskid;

        // pushing the tail to next emty queue
        pool->tail = next;
        pool->queue_counter++;

        // Signal a waiting worker that the queue has been updated
        // and the lock will be released for it to proceed.
        if(pthread_cond_signal(&(pool->notify)) < 0) {
            printf("Thread broadcast wasn't successfull.\n");
            err = -1;
            break;
        }
    } while(0);

    // lock is now released
    if(pthread_mutex_unlock(&(pool->lock)) < 0){
        printf("Thread mutex unlock failed.\n");
        err = -1;
    }

    return err;
}

int f_tpool_destroy(tpool_t *pool){

    if(pool == NULL){
        printf("Invalid threadpool.\n");
        return -1;
    }

    if(pthread_mutex_lock(&(pool->lock)) < 0) {
        printf("Mutex lock failed.\n");
        return -1;
    }

    if(pool->stop != 0){
        printf("please set the value of stop as true to stop the threads.\n");
        return -1;
    }

    // waking up the worker threads
    pthread_cond_broadcast(&(pool->notify));
    pthread_mutex_unlock(&(pool->lock));


    // joining all the worker threads
    for(int i = 0; i < pool->starting_threads; i++){
        if(pthread_join(pool->worker_threads[i], NULL) < 0) {
            printf("Pool deallocation failed.\n");
            return -1;
        }
    }

    return f_tpool_free(pool);
}


int f_tpool_free(tpool_t *pool){
    if(pool == NULL){
        printf("Invalid threadpool.\n");
        return -1;
    }

    if(pool->worker_threads != NULL){
        free(pool->worker_threads);
    }

    if(pool->queue != NULL){
        free(pool->queue);
    }


    pthread_mutex_destroy(&(pool->lock));
    pthread_cond_destroy(&(pool->notify));

    free(pool);
    free(tout);
    return 0;
}

static void *f_worker_thread(void *tpool){
    s_tpool_task task;
    tpool_t *pool = (tpool_t *)tpool;

    while(1){
        pthread_mutex_lock(&(pool->lock));

        while((pool->starting_threads == 0) && (pool->stop != 1)){
            pthread_cond_wait(&(pool->notify), &(pool->lock));
        }

        if((pool->stop == 1) && (pool->starting_threads == 0)){
            break;
        }

        // Getting tasks
        task.func = pool->queue[pool->head].func;
        task.argument = pool->queue[pool->head].argument;
        task.task_id = pool->queue[pool->head].task_id;

        pool->head = (pool->head + 1) % pool->queue_size;

        pool->queue_counter--;


        pthread_mutex_unlock(&(pool->lock));

        // starting the work
        task.func(task.argument);

        // updating the task status as complete
        pthread_mutex_lock(&tout->tlock);
        tout->out_queue[tout->tail].task_id = task.task_id;
        tout->out_queue[tout->tail].status = TASK_FINISHED;

        // moving to the next buffer
        tout->tail = (tout->tail + 1) % tout->queue_size;
        tout->queue_counter++;

        pthread_cond_signal(&(tout->tnotify), NULL);
        pthread_mutex_unlock(&(tout->tlock));

    }

    pool->starting_threads--;

    pthread_mutex_unlock(&(pool->lock));
    pthread_exit(NULL);
    return NULL;
}

int f_tpool_done(tpool_t *pool, TaskOut *tout, int maxoutput){
    // TODO
}


