#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

void child(int pid, int period, int deadline, char * name) {
  user_set_edf(pid,period,deadline);
  printf( "%s running with deadline %d\n", name, deadline);
  sleep(50);  // simulate work
  printf( "%s done\n", name);
//   exit();
}

int main() {

  if (fork() == 0) {
    int pida = fork();
    child(pida, 10, 20, "Process C");
  }

  if (fork() == 0) {
    int pidb = fork();
    child(pidb,5, 10, "Process A");
  }

  if (fork() == 0) {
    int pidc = fork();
    child(pidc,6,36, "Process B");
  }

//   // Parent waits for all to finish
//   wait();
//   wait();
//   wait();

  printf("EDF test complete\n");
//   exit();
}
