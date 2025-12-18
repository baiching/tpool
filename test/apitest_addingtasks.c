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

    int sq = (*n) * (*n);
    printf("The squre if %d is: %d.\n", *n, sq);
}

int main() {
    tpool_t *pool;
    uint32_t tid[3];
    // TaskOut completed[10];

    printf("=== Starting threadpool test ===\n");

    pool = f_tpool_create(1, 10);

    if(pool == NULL){
        printf("Threadpool creation has failed.\n");
        return -1;
    }

    //printf("SUCCESS: Threadpool created at address %p\n", (void*)pool);

    int val = 5;

    f_tpool_add_task(pool, tid, square, &val);


    #ifdef _WIN32
        Sleep(10);
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
