# CPools

CPools is an easy, light-weight and fast C thread pool library, using a cross-platform thread library.

## Features

- Cross-platform
- No dynamic memory allocation
- Simple API

## Notes

This library uses a single thread for now, due to be still WIP, although it is already usable, but won't have support for that, for now.

And cross-platform hasn't been tested yet, but it should work on any platform that supports pthreads.
If you find *any* issue on Windows, create an issue on GitHub.

## Usage

### Creating a pool

```c
#include <cpools.h>

int main() {
  // Create a pool with with 10 max operations, with a 100ms delay for each operation
  struct cpools_pool cpools;
  struct cpools_operation operations[10] = { 0 };

  cpolls_init(&cpools, operations, 10, 100);
}
```

### Adding an operation and running the pool

```c
void *operation(void *data) {
  printf("Operation completed! ID: %s\n", (char *)data);

  return (void *)"Operation completed!";
}

void done(void *data) {
  printf("Completed operation, returned: %s\n", (char *)data);

  return;
}

...

  // Starts running the thread pool
  cpolls_run(&cpools);

  // Add 2 operations to the pool
  cpolls_add_job(&cpools, operation, done, (void *)"1");
  cpolls_add_job(&cpools, operation, done, (void *)"2");

  // Stops pool
  cpolls_stop(&cpolls);
```

Due to not using any dynamic memory allocation, you won't need to free any memory, as it is all stack allocated.

### Output

```text
Operation completed! ID: 1
Completed operation, returned: Operation completed!
Operation completed! ID: 2
Completed operation, returned: Operation completed!
```
