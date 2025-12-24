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
 * 5. int f_tpool_destroy(pool): gracefully destroys the threadpool
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
 * @brief Generate a unique task identifier
 *
 * @return Unique 32-bit task identifier
 *
 * @note Thread-safe: safe to call from multiple threads concurrently.
 * @note Must be called to generate task IDs before calling f_tpool_add_task().
 * @note The identifier is monotonically increasing until wrap-around at 2^32-1.
 *
 * @see f_tpool_add_task
 */
uint32_t f_tpool_get_taskid(void);

/**
 * @brief Create a new threadpool instance
 *
 * @param[in] num_of_threads Number of worker threads (1 to MAX_THREADS)
 * @param[in] queue_size     Maximum pending tasks (1 to MAX_QUEUE)
 *
 * @return Pointer to new threadpool, or NULL on failure
 *
 * @note Threads start immediately and wait for tasks.
 * @note Use f_tpool_destroy() to clean up the threadpool.
 *
 * @warning Ensure queue_size is sufficient for expected workload to avoid
 *          f_tpool_add_task() returning "queue full" errors.
 *
 * @see f_tpool_destroy
 */
tpool_t *f_tpool_create(int num_of_threads, int queue_size);


/**
 * @brief Submit a task to the threadpool for execution
 *
 * @param[in] pool      Threadpool instance (must be initialized)
 * @param[in] taskid    Unique task identifier (use f_tpool_get_taskid())
 * @param[in] function  Function to execute in worker thread
 * @param[in] arg       Argument passed to the function (may be NULL)
 *
 * @retval  0   Task successfully queued
 * @retval -1   Error: invalid parameters, pool stopped, or queue full
 *
 * @note The function will be called asynchronously in a worker thread.
 * @note The argument pointer must remain valid until the task completes.
 * @note Use f_tpool_get_taskid() to generate unique task identifiers.
 *
 * @see f_tpool_get_taskid
 * @see f_tpool_create
 */
int f_tpool_add_task(tpool_t *pool, uint32_t taskid, void ( *function)(void *), void *arg);

/**
 * @brief Retrieve completed task results (non-blocking)
 *
 * @param[out] tasks          Array to receive completed task information
 * @param[in]  maxoutput      Maximum number of tasks to retrieve (size of tasks array)
 *
 * @retval  n > 0   Number of tasks successfully retrieved
 * @retval  0       No tasks completed yet
 * @retval -1       Error: invalid parameters or buffer not initialized
 *
 * @note This function is non-blocking. Returns immediately even if no tasks
 *       are completed. Caller should implement polling if waiting is required.
 * @note Retrieved tasks are removed from the output buffer.
 *
 * @see f_tpool_add_task
 * @see init_completion_buffer
 */
int f_tpool_done(TaskOut *task, int maxoutput);


/**
 * @brief Gracefully shutdown and destroy a threadpool
 *
 * @param[in] pool Threadpool to destroy (must be initialized)
 *
 * @retval  0   Threadpool successfully destroyed
 * @retval -1   Error: invalid pool or destruction failed
 *
 * @note This function performs a graceful shutdown:
 *       1. Stops accepting new tasks
 *       2. Waits for all queued tasks to complete
 *       3. Signals worker threads to exit
 *       4. Waits for worker threads to terminate
 *       5. Frees all associated resources
 *
 * @warning Blocks until all pending tasks finish execution.
 * @warning Do not call from within a worker thread (deadlock).
 *
 * @see f_tpool_create
 */
int f_tpool_destroy(tpool_t *pool);

#ifdef __cplusplus
}
#endif


#endif
