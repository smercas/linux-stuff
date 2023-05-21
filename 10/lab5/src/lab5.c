#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int32_t main(int32_t argc, char** argv) {
  uint64_t n, i;
  pid_t pid;
  if (argc - 1 != 1) { fprintf(stderr, "usage: n\n"); return -1; }
  n = strtoull(argv[1], NULL, 10);
  printf("Initial process PID: %d, parent process' PID: %d.\n", getpid(), getppid());
  //  branching into n + 1 processes (1 parent, n children)
  for (i = 1; i <= n; ++i) {
    switch (pid = fork()) {
      case -1:
        perror("fork");
        return -1;
        break;
      case  0:
        printf("Child process number %lu (PID = %d, parent process' PID = %d).\n", i, getpid(), getppid());
        return i;   //IMPORTANT: child process mustn't continue executing the for loop in order for him to not create more processes
      default:
        break;
    }
  }
  // wait for all n child processes to end
  for (i = 1; i <= n; ++i) {         //  parent process will wait all the child processes to end(some might have already ended)
    if (wait(NULL) == -1) { perror("wait"); return -1; }   //  some child processes might apprear as orphans(parent process' PID is 1)
  }
  return 0;
}
