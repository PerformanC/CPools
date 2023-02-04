#include <stdio.h>

#include "../cpools.h"

void *operation(void *data) {
  printf("Operation completed! ID: %s\n", (char *)data);

  return (void *)"Operation completed!";
}

void done(void *data) {
  printf("Completed operation, returned: %s\n", (char *)data);

  return;
}

int main(void) {
  struct cpools_pool cpolls;
  struct cpools_operation operations[10] = { 0 };
  cpolls_init(&cpolls, operations, 10, 100);

  cpolls_run(&cpolls);

  cpolls_add_job(&cpolls, operation, done, (void *)"1");
  cpolls_add_job(&cpolls, operation, done, (void *)"2");

  cpolls_stop(&cpolls);

  return 0;
}
