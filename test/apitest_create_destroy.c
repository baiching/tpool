#include <stdio.h>
#include "tpool.h"

#define THREADS 4
#define QUEUE 128

int main(){
    tpool_t *pool;

    printf("=== Starting threadpool test ===\n");

    pool = f_tpool_create(2, 10);

    if(pool == NULL){
        printf("Threadpool creation has failed.\n");
        return -1;
    }

    printf("SUCCESS: Threadpool created at address %p\n", (void*)pool);

    if(f_tpool_destroy(pool) < 0){
        printf("failed to destroy the threadpool.\n");
        return -1;
    }

    printf("=== Test: Threadpool creation destroy PASSED. ===\n");

    return 0;
}
