/**
 * A Simple Threadpool with Completion
 * @author Uthowaipru Chowdhury Baiching
 *
 * API List:
 * 1. uint32_t f_get_taskid(void) : a unique task id which is required for adding the task in the threadpool
 * 2. tpool_t *f_tpool_create( num_of_threads, queue_size): Creates the thread pool and
 *                                                                 returns the newly created threadpool.
 * 3. int f_tpool_add_task(pool, taskid, function pointer, argument): adds task it the tpool queue
 * 4. int f_tpool_done(*task, maxoutput): retrives the completed tasks status with their ID's
 * 5. int f_tpool_destroy(pool): destroys the threadpool
 *
 */

#ifndef TPOOL_H
#define TPOOL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_THREADS 64
#define MAX_QUEUE 65536

/**
 * For status of tasks in the queue
 *
 */
typedef enum {
    TASK_PENDING,
    TASK_ONGOING,
    TASK_FINISHED
}e_task_status;

//typedef enum e_task_status e_task_status;

typedef struct tpool_t tpool_t;

/**
 * @brief It's exclusively for serving executed tasks taskid on completion
 *
 * @var task_id: the unique identifier for each task
 * @var status: current state of the task
 * @var queue_size: the size of the ring buffer for tasks
 * @var queue_counter: the number of tasks in the queue
 *
 */
typedef struct{
    uint32_t task_id;
    e_task_status status;
}TaskOut;


/**
 * @breif Returns a unique task id(thread safe)
 *
 *
 */
uint32_t f_tpool_get_taskid(void);

/**
 * @brief Creates the thread pool
 *
 * @param num_of_threads: the number of threads you want to be created
 * @param queue_size:     the size of the queue for tasks
 *
 * @return a new threadpool or a NULL value with a printf stating the error
 *
 */
tpool_t *f_tpool_create(int num_of_threads, int queue_size);


/**
 * @brief Adds new taks in the queue
 *
 * @param pool:         the pointer to the thread pool
 * @param function:     the function pointer points to the function that needed to run
 * @param arg:          the argument that needs to be passed to the function
 * @param taskid:       taskid for individual tasks, f_get_taskid() provides it
 *
 * @return 0 on Success and -1 on failure
 *
 */
int f_tpool_add_task(tpool_t *pool, uint32_t taskid, void ( *function)(void *), void *arg);

/**
 * @brief Adds new taks in the queue
 *
 *
 * @param task:         It's size is needed to be equal to the queue_size parameter for tpool creation
 * @param maxoutput:    total results could be returned at once
 *
 * @return              number of completed tasks. On failure it returns -1
 *
 */
int f_tpool_done(TaskOut *task, int maxoutput);


/**
 * @brief Finishes all unfished tasks and destroy the thread pool
 *
 * @param pool:         the thread pool that required to destroy
 *
 * @return              0 on Success and -1 on failure
 *
 */
int f_tpool_destroy(tpool_t *pool);

#ifdef __cplusplus
}
#endif


#endif
