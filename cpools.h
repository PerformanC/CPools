#ifndef CPOOLS_H
#define CPOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cthreads.h"

#ifdef __unix__
#include <unistd.h>
#elif _WIN32
#include <windows.h>
#endif

#define CPOOLS_RUNNING 1
#define CPOOLS_STOPPING 2

#define CPOOLS_EMPTY 0
#define CPOOLS_PROCESSING 1
#define CPOOLS_QUEUED 2

struct cpools_operation {
  void *(*function)(void *data);
  void (*callback)(void *data);
  void *data;
  int state;
};

struct cpools_pool {
  struct cthreads_thread *threads;
  struct cpools_operation *operations;
  size_t threads_count;
  size_t operations_count;
  size_t operations_delay;
  size_t max_operations;
  int state;
};

#define cpools_thread cthreads_thread

void cpools_init(struct cpools_pool *cpools, struct cpools_operation operations[], size_t max_capacity, size_t delay) {
  unsigned int i = 0;

  cpools->operations = operations,
  cpools->operations_count = 0;
  cpools->operations_delay = delay;
  cpools->max_operations = max_capacity;
  cpools->state = CPOOLS_RUNNING;

  while (i < max_capacity) {
    cpools->operations[i].function = NULL;
    cpools->operations[i].callback = NULL;
    cpools->operations[i].data = NULL;
    cpools->operations[i].state = CPOOLS_EMPTY;

    i++;
  }

  return;
}

int cpools_add_job(struct cpools_pool *cpools, void *(*function)(void *data), void (*callback)(void *data), void *data) {
  if (cpools->max_operations == cpools->operations_count) return 1;

  cpools->operations[cpools->operations_count].function = function;
  cpools->operations[cpools->operations_count].callback = callback;
  cpools->operations[cpools->operations_count].data = data;
  cpools->operations[cpools->operations_count].state = CPOOLS_QUEUED;
  cpools->operations_count++;

  return 0;
}

void *__cpools_run_thread(void *data) {
  struct cpools_pool *cpools = (struct cpools_pool *)data;

  unsigned int i = 0;
  while (cpools->state == CPOOLS_RUNNING) {
    if (cpools->operations[i].state == CPOOLS_QUEUED) {
      cpools->operations[i].state = CPOOLS_PROCESSING;
      cpools->operations[i].callback(cpools->operations[i].function(cpools->operations[i].data));
      cpools->operations[i].state = CPOOLS_EMPTY;
      cpools->operations_count--;
    }

    if (i++ == cpools->max_operations) {
      i = 0;

      #ifdef __unix__
        sleep(cpools->operations_delay / 1000);
      #elif _WIN32
        Sleep(cpools->operations_delay);
      #endif
    }
  }

  return data;
}

void cpools_run(struct cpools_pool *cpools, struct cpools_thread *threads, size_t thread_count) {
  unsigned int i = 0;

  cpools->threads = threads;
  cpools->threads_count = thread_count;

  while (i < thread_count) {
    cthreads_thread_create(&cpools->threads[i], NULL, __cpools_run_thread, (void *)cpools, NULL);

    i++;
  }

  return;
}

void cpools_stop(struct cpools_pool *cpools) {
  unsigned int i = 0;

  cpools->state = CPOOLS_STOPPING;

  while (i < cpools->operations_count) {
    cpools->operations[i].function = NULL;
    cpools->operations[i].callback = NULL;
    cpools->operations[i].data = NULL;
    cpools->operations[i].state = CPOOLS_EMPTY;

    i++;
  }

  while (i < cpools->threads_count) {
    cthreads_join(&cpools->threads[i], NULL);

    i++;
  }

  return;
}

#ifdef __cplusplus
}
#endif

#endif
