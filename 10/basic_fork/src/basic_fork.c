#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>

int32_t main() {
  pid_t child_pid;
  switch (child_pid = fork()) {
    case -1:  //error
      perror("fork");
      return -1;
      break;
    case 0:   //child
      //what children do
      return 0;
      break;
    default:  //parent
      //what parents do
      break;
  }
  if (wait(NULL) == -1) { perror("wait"); return -1; }
  return 0;
}
