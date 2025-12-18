/**
 * To test if the task gets added and executed
 */

#include <stdio.h>
#include <stdint.h>
#include <windows.h>
#include "tpool.h"

#define THREADS 4
#define QUEUE 128

void square(void *arg){
    int *n = (int *)arg;

    // *n = (*n) * (*n);
    printf("The squre if %d is: %d.\n", *n, (*n) * (*n));
    free(arg);
}

int main() {
    tpool_t *pool;
    uint32_t tid[15];
    TaskOut completed[10];

    printf("=== Starting threadpool test ===\n");

    pool = f_tpool_create(4, 64);

    if(pool == NULL){
        printf("Threadpool creation has failed.\n");
        return -1;
    }

    //printf("SUCCESS: Threadpool created at address %p\n", (void*)pool);

    int val = 5;

    for(int i = 0; i < 15; i++){
        int *data = malloc(sizeof(int));
        *data = i;
        tid[i] = f_tpool_get_taskid();
        f_tpool_add_task(pool, tid[i], square, data);
        printf("Added task %u\n", tid[i]);
    }

    printf("\nPolling for completion (50ms intervals):\n");
    int remaining = 15;
    int attempts = 0;
    #ifdef _WIN32
        Sleep(100);
    #else
        sleep(1);
    #endif

    while (attempts < 5000) {  // Max 50 attempts = 2.5 seconds
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

        attempts++;
    }

    //f_tpool_add_task(pool, tid, square, &val);


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
