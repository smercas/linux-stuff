#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>     //wait
#include <stdlib.h>       //abort
#include <stdint.h>

//  0 - normal
//  1 - abnormal
//  2 - infinite for loop
#define behaviour 2


int32_t main() {
  pid_t child_pid;

  //  the creation of a child process
  child_pid = fork();

  // execution branching
  switch (child_pid) {
    case -1:
      perror("fork");
      return -1;
      break;
    case 0:
      //  This code sequence will be executed only by child processes
      printf("Child process (PID = %d).\n\n", getpid());
#if behaviour == 0
      return 3; //  could also use exit(3);
#elif behaviour == 1
      abort();
#else
      for (;;);
      //  use either one of the following commands to kill the child process
      //  > kill -INT child_pid
      //  > kill -TERM child_pid
      //  > kill -ABRT child_pid
#endif
      break;
    default:

      int32_t status;

      child_pid = wait(&status);
      //  his code sequence will only be executed by the parent process
      printf("Parent process (PID = %d): the child process with the PID of %d has ended with the following status: %d.\n\n", getpid(), child_pid, status);

      if (child_pid == -1) { perror("wait"); return -1; }

      //  extracting the termination code from the "high" byte of the status
      //  and the signal number from the "low" byte of the status
      /*
      int termination_code = status >> 8;
      if (termination_code != 0) {
        printf("The child process with the PID of %d has been terminated normally, with the following termination code: %d", child_pid, termination_code);
      }
      int signal = status & 0xFF;
      if (signal != 0) {
        printf("The child process with the PID of %d has been terminated abnormally, due to the following signal: %d", child_pid, signal);
      }
      */
      //alternatively, the above code sequence can be written as:
      if (WIFEXITED(status)) {
        printf("The child process with the PID of %d has been terminated normally, with the following termination code: %d\n\n", child_pid, WEXITSTATUS(status));
      }
      if (WIFSIGNALED(status)) {
        printf("The child process with the PID of %d has been terminated abnormally, due to the following signal: %d\n\n", child_pid, WTERMSIG(status));
      }

      break;
  }
}
