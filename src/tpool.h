#ifndef TPOOL_H
#define TPOOL_H


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_THREADS 64
#define MAX_QUEUE 65536

typedef struct tpool_t tpool_t;


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
 *
 * @return 0 on Success and -1 on failure
 *
 */
int f_tpool_add(tpool_t *pool, void (*function)(void *), void *arg);

/**
 * @brief Adds new taks in the queue
 *
 * @param pool:         the pointer to the thread pool
 * @param output:       It points to the completed tasks results
 * @param maxoutput:    total results could be returned at once
 *
 * @return              number of completed tasks
 *
 */
int f_tpool_done(tpool_t *pool, void **output, int maxoutput);


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
