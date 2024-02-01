#include <stdio.h>
#include <unistd.h>
#include "../cthreads.h"

#include "../cpools.h"

void *operation(void *data) {
  printf("Processing operation: %s by %lu\n", (char *)data, cthreads_thread_id(cthreads_thread_self()));

  sleep(2);

  printf("Operation completed! ID: %s\n", (char *)data);

  return (void *)"Operation completed!";
}

void done(void *data) {
  printf("Completed operation, returned: %s\n", (char *)data);

  return;
}

int main(void) {
  struct cpools_pool cpools;
  struct cpools_operation operations[10] = { 0 };
  struct cpools_thread threads[2] = { 0 };
  cpools_init(&cpools, operations, 10, 100);

  cpools_run(&cpools, threads, 2);

  printf("Adding jobs...\n");
  cpools_add_job(&cpools, operation, done, (void *)"1");
  printf("Added job 1\n");
  cpools_add_job(&cpools, operation, done, (void *)"2");
  printf("Added job 2\n");

  while (cpools.operations_count != 0) { /* Noop */ }

  cpools_stop(&cpools);

  return 0;
}
