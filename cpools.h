#ifndef CPOOLS_H
#define CPOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <time.h>

#include "cthreads.h"

#ifdef __unix__
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

struct cpools_operation {
  void *(*function)(void *data);
  void (*callback)(void *data);
  void *data;
};

struct cpools_pool {
  struct cthreads_thread *thread;
  struct cpools_operation *operations;
  size_t operation_count;
  size_t operations_delay;
  size_t max_operations;
};

void cpolls_init(struct cpools_pool *cpools, struct cpools_operation operations[], size_t max_capacity, size_t delay) {
  cpools->operations = operations,
  cpools->operation_count = 0;
  cpools->operations_delay = delay;
  cpools->max_operations = max_capacity;

  return;
}

int cpolls_add_job(struct cpools_pool *cpools, void *(*function)(void *data), void (*callback)(void *data), void *data) {
  if (cpools->max_operations == cpools->operation_count) return 1;

  cpools->operations[cpools->operation_count].function = function;
  cpools->operations[cpools->operation_count].callback = callback;
  cpools->operations[cpools->operation_count].data = data;
  cpools->operation_count++;

  return 0;
}

void *__cpolls_run_thread(void *data) {
  struct cpools_pool *cpools = (struct cpools_pool *)data;

  int operations_delay = cpools->operations_delay;
  while (1) {
    if (cpools->operation_count != 0) {
      cpools->operations[cpools->operation_count - 1].callback(cpools->operations[cpools->operation_count - 1].function(cpools->operations[cpools->operation_count - 1].data));

      cpools->operations[cpools->operation_count - 1].function = NULL;
      cpools->operations[cpools->operation_count - 1].callback = NULL;
      cpools->operations[cpools->operation_count - 1].data = NULL;

      cpools->operation_count--;
    }
    #ifdef __unix__
      sleep(operations_delay / 1000);
    #elif _WIN32
      Sleep(>operations_delay);
    #endif
  }

  return data;
}

void cpolls_run(struct cpools_pool *cpools) {
  struct cthreads_thread thread;
  cpools->thread = &thread;

  cthreads_thread_create(cpools->thread, __cpolls_run_thread, (void *)cpools);

  return;
}

void cpolls_stop(struct cpools_pool *cpools) {
  cthreads_thread_close(cpools->thread);

  return;
}

#ifdef __cplusplus
}
#endif

#endif
