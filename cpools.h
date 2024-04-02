#ifndef CPOOLS_H
#define CPOOLS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cthreads.h"

#define CPOOLS_RUNNING  1
#define CPOOLS_STOPPING 2

#define CPOOLS_EMPTY      0
#define CPOOLS_PROCESSING 1
#define CPOOLS_QUEUED     2

struct cpools_operation {
  void *(*function)(void *data);
  void (*callback)(void *data);
  void *data;
  struct cthreads_mutex *mutex;
  int state;
};

struct cpools_thread {
  struct cthreads_thread thread;
  struct cthreads_mutex *mutex;
  struct cthreads_mutex *info_mutex;
  struct cpools_operation *operations;
  size_t operations_count;
  size_t operations_capacity;
  int state;
};

struct cpools_pool {
  struct cpools_thread *threads;
  size_t threads_amount;
  size_t operations_count;
  size_t operations_capacity;
};

void cpools_init(struct cpools_pool *cpools, size_t threads_amount, size_t operations_capacity) {
  unsigned int i = 0, j = 0;

  cpools->operations_count = 0;
  cpools->operations_capacity = operations_capacity;
  cpools->threads = (struct cpools_thread *)malloc(sizeof(struct cpools_thread) * threads_amount);
  cpools->threads_amount = threads_amount;
  
  while (i < threads_amount) {
    cpools->threads[i].operations = (struct cpools_operation *)malloc(sizeof(struct cpools_operation) * operations_capacity);
    cpools->threads[i].operations_count = 0;
    cpools->threads[i].state = CPOOLS_EMPTY;
    cpools->threads[i].operations_capacity = operations_capacity;
    cpools->threads[i].mutex = (struct cthreads_mutex *)malloc(sizeof(struct cthreads_mutex));
    cpools->threads[i].info_mutex = (struct cthreads_mutex *)malloc(sizeof(struct cthreads_mutex));

    cthreads_mutex_init(cpools->threads[i].mutex, NULL);
    cthreads_mutex_init(cpools->threads[i].info_mutex, NULL);

    while (j < operations_capacity) {
      cpools->threads[i].operations[j].function = NULL;
      cpools->threads[i].operations[j].callback = NULL;
      cpools->threads[i].operations[j].data = NULL;
      cpools->threads[i].operations[j].mutex = malloc(sizeof(struct cthreads_mutex));

      cthreads_mutex_init(cpools->threads[i].operations[j].mutex, NULL);

      j++;
    }

    j = 0;

    i++;
  }

  return;
}

int cpools_add_job(struct cpools_pool *cpools, void *(*function)(void *data), void (*callback)(void *data), void *data) {
  unsigned int i = 0, least_used = 0;

  while (i < cpools->threads_amount) {
    struct cpools_thread *cpools_thread = &cpools->threads[i];

    if (cpools_thread->operations_count < cpools->threads[least_used].operations_count) {
      least_used = i;

      if (cpools->threads[least_used].operations_count == 0) break; /* No need to continue if we found an idle thread */
    }

    i++;
  }

  i = 0;

  while (i < cpools->threads[least_used].operations_capacity) {
    struct cpools_operation *operation = &cpools->threads[least_used].operations[i];

    if (cthreads_mutex_trylock(operation->mutex) == 0 && operation->state == CPOOLS_EMPTY) {
      operation->function = function;
      operation->callback = callback;
      operation->data = data;
      operation->state = CPOOLS_QUEUED;
      cthreads_mutex_lock(cpools->threads[least_used].info_mutex);

      cpools->threads[least_used].operations_count++;

      cthreads_mutex_unlock(cpools->threads[least_used].info_mutex);

      cthreads_mutex_unlock(operation->mutex);

      if (cpools->threads[least_used].operations_count == 1) {
        cthreads_mutex_unlock(cpools->threads[least_used].mutex);
      }

      return 0;
    }

    i++;
  }

  return 0;
}

void *__cpools_run_thread(void *data) {
  unsigned int i = 0;
  struct cpools_thread *cpools_thread = (struct cpools_thread *)data;

  while (1) {
    cthreads_mutex_lock(cpools_thread->mutex);

    while (cpools_thread->operations_count != 0) {
      struct cpools_operation *operation = &cpools_thread->operations[i];

      cthreads_mutex_lock(operation->mutex);

      cpools_thread->operations[i].callback(cpools_thread->operations[i].function(cpools_thread->operations[i].data));
      cpools_thread->operations[i].state = CPOOLS_EMPTY;

      cthreads_mutex_lock(cpools_thread->info_mutex);

      cpools_thread->operations_count--;

      cthreads_mutex_unlock(cpools_thread->info_mutex);

      cthreads_mutex_unlock(operation->mutex);

      if (i == cpools_thread->operations_capacity - 1) i = 0;
      else i++;
    }

    /* "Dead-lock" to wait for new operations */
  }

  return data;
}

void cpools_run(struct cpools_pool *cpools) {
  unsigned int i = 0;

  while (i < cpools->threads_amount) {
    struct cpools_thread *cpools_thread = &cpools->threads[i];

    /* Lock the thread to prevent it from running before we add operations */
    cthreads_mutex_lock(cpools_thread->mutex);

    cthreads_thread_create(&cpools->threads[i].thread, NULL, __cpools_run_thread, (void *)cpools_thread, NULL);

    i++;
  }

  return;
}

void cpools_stop(struct cpools_pool *cpools) {
  unsigned int i = 0, j = 0;

  while (i < cpools->threads_amount) {
    cthreads_thread_cancel(cpools->threads[i].thread);
    cthreads_mutex_destroy(cpools->threads[i].mutex);
    free(cpools->threads[i].mutex);
    cthreads_mutex_destroy(cpools->threads[i].info_mutex);
    free(cpools->threads[i].info_mutex);

    while (j < cpools->threads[i].operations_capacity) {
      cthreads_mutex_destroy(cpools->threads[i].operations[j].mutex);
      free(cpools->threads[i].operations[j].mutex);

      j++;
    }

    free(cpools->threads[i].operations);

    i++;
  }

  return;
}

#ifdef __cplusplus
}
#endif

#endif
