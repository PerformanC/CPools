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
  cpools_init(&cpools, 2, 3);

  cpools_run(&cpools);

  printf("Adding jobs...\n");
  cpools_add_job(&cpools, operation, done, (void *)"1");
  printf("Added job 1\n");
  cpools_add_job(&cpools, operation, done, (void *)"2");
  printf("Added job 2\n");

  while (1) {
    size_t i = 0;

    while (i < cpools.threads_amount) {
      struct cpools_thread *cpools_thread = &cpools.threads[i];

      cthreads_mutex_lock(cpools_thread->info_mutex);

      if (cpools_thread->operations_count == 0) {
        cthreads_mutex_unlock(cpools_thread->info_mutex);

        break;
      }

      i += cpools_thread->operations_count;

      cthreads_mutex_unlock(cpools_thread->info_mutex);
    }

    if (i == 0) break;

    sleep(1); /* Avoid high CPU usage */
  }

  cpools_stop(&cpools);

  return 0;
}
