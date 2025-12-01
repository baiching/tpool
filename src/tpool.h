#ifndef TPOOL_H
#define TPOOL_H


#ifdef __cplusplus
extern "C" {
#endif

#define MAX_THREADS 64
#define MAX_QUEUE 65536

typedef struct tpool_t tpool_t;

tpool_t *f_tpool_create(int num_of_threads, int queue_size);

int f_tpool_add(tpool_t *pool, void (*function)(void *, void *));

int f_tpool_done(tpool_t *pool, int maxoutput, int timeout);

int f_tpool_destroy(tpool_t *pool);

#ifdef __cplusplus
}
#endif


#endif
