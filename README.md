# ThreadPool Library
A simple C/C++ threadpool implementation with task completion tracking.

# Tool Requirements
* Mingw gcc compiler 6.3 and above
* mingw32-make

## Build and Run 
* Create a new directory named buid
* Go to build directory
```bash 
cd build
```
* To configure 
```bash
cmake -G "MinGW Makefiles" ..
```
* To build the project
```
cmake --build .
```

## API Reference
```c
uint32_t f_get_taskid(void)
```
Returns a unique thread-safe task ID.

```c
tpool_t* f_tpool_create(int num_of_threads, int queue_size)
```
Creates a threadpool.

num_of_threads: Number of worker threads (max: MAX_THREADS=64)

queue_size: Task queue size (max: MAX_QUEUE=65536)

Returns: Threadpool pointer or NULL on error

```c
int f_tpool_add_task(tpool_t* pool, uint32_t taskid, void (*function)(void*), void* arg)
```
Adds a task to the queue.

pool: Threadpool pointer

taskid: Unique task ID (use f_get_taskid())

function: Function to execute

arg: Argument passed to function

Returns: 0 on success, -1 on failure
```c
int f_tpool_done(TaskOut* task, int maxoutput)
```
Retrieves completed tasks.

task: Array of TaskOut (size ≥ queue_size)

maxoutput: Maximum results to return

Returns: Number of completed tasks, -1 on failure
```c
int f_tpool_destroy(tpool_t* pool)
```
Destroys threadpool, finishing all pending tasks.

Returns: 0 on success, -1 on failure

Limits
Max threads: 64

Max queue size: 65536

Notes
Thread-safe design

C11 and upwards compatible

All pending tasks complete before pool destruction
