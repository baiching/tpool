/**
 * To test if the task gets added and executed
 */

#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "tpool.h"

//#define THREADS 4
#define QUEUE 128

void square(void *arg){
    int *n = (int *)arg;

    // *n = (*n) * (*n);
    printf("The squre if %d is: %d.\n", *n, (*n) * (*n));
    free(arg);
}

int main() {
    tpool_t *pool;
    uint32_t tid;
    TaskOut completed[5];

    printf("=== Starting threadpool test ===\n");

    pool = f_tpool_create(2, 64);

    if(pool != NULL){
        printf("Threadpool created.\n");
        //return -1;
    }

    //printf("SUCCESS: Threadpool created at address %p\n", (void*)pool);

    for(int i = 0; i < 5; i++){
        int *data = malloc(sizeof(int));
        *data = i;
        tid = f_tpool_get_taskid();
        f_tpool_add_task(pool, tid, square, data);
        printf("Added task %u\n", tid);
    }

    printf("\nPolling for completion (50ms intervals):\n");
    int remaining = 5;
    int attempts = 0;
    #ifdef _WIN32
        Sleep(100);
    #else
        sleep(1);
    #endif
    int x = 0;
    while (remaining > 0) {
        int retrieved = f_tpool_done(completed, 10);

        if (retrieved < 0) {
            printf("Error: f_tpool_done() returned %d\n", retrieved);
            break;
        }

        if (retrieved > 0) {
            printf("Attempt %2d: %d task(s) completed -> ", attempts, retrieved);
            for (int i = 0; i < retrieved; i++) {
                printf("%u ", completed[i].task_id);
            }
            printf("\n");
            remaining -= retrieved;
        }

        //printf("inside the loop. %d \n", x++);
    }

    printf("left the loop.\n");
    #ifdef _WIN32
        Sleep(100);
    #else
        sleep(1);
    #endif

    if(f_tpool_destroy(pool) < 0){
        printf("failed to destroy the threadpool.\n");
        return -1;
    }

    printf("=== Test2: Threadpool creation destroy PASSED. ===\n");

    return 0;
}
