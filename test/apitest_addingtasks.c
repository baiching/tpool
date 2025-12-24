/**
 * To test if the task gets added and executed
 */

#include <stdio.h>
#include <stdint.h>
#include "tpool.h"


void square(void *arg){
    int *n = (int *)arg;

    //int sq = (*n) * (*n);
    printf("The squre if %d is: %d.\n", *n,  (*n) * (*n));
}

int main() {
    tpool_t *pool;
    uint32_t tid;
    // TaskOut completed[10];

    printf("=== Starting threadpool test ===\n");

    tid = f_tpool_get_taskid();

    pool = f_tpool_create(1, 10);

    if(pool == NULL){
        printf("Threadpool creation has failed.\n");
        return -1;
    }

    //printf("SUCCESS: Threadpool created at address %p\n", (void*)pool);

    int *val = malloc(sizeof(int));
    *val = 5;

    f_tpool_add_task(pool, tid, square, val);
    TaskOut completed[10];
    f_tpool_done(completed, 2);
    for(int i = 0; i < 10; i++){
        printf("%d\n", completed[i].task_id);
    }

    if(f_tpool_destroy(pool) < 0){
        printf("failed to destroy the threadpool.\n");
        return -1;
    }

    printf("=== Test2: Threadpool creation destroy PASSED. ===\n");

    return 0;
}
