#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

int32_t main() {
  pid_t child_pid;
  int32_t number = 0;
  //  the creation of a child process
  child_pid = fork();
  //  execution branching
  switch (child_pid) {
    case -1:
      perror("fork");
      return -1;
      break;
    case 0:
      //  This code sequence will be executed only by child processes
      printf("Child process' PID: %d\nIts parent's PID: %d.\n\n", getpid(), getppid());
      printf("Within the child process: shortly after calling fork(), the 'number' variable will have the following value: %d.\n", number);
      number = 5;
      printf("Within the child process: after modifying the 'number' variable, it'll will have the following value: %d.\n", number);
      break;
    default:
      //  his code sequence will only be executed by the parent process
      printf("Parent process' PID: %d\nIts parent's PID: %d;\nIts child's PID: %d.\n\n", getpid(), getppid(), child_pid);
      sleep(2);
      printf("Within the parent process: 2 seconds after branching, the 'number' variable will have the following value: %d.\n", number);
      break;
  }
  //  Shared code sequence, executed by both processes
  printf("Shared code sequence, executed by the %s process.\n\n", child_pid == 0 ? "child" : "parent");
  return 0;
}
