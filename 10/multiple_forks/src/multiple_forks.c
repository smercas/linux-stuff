#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>

int32_t main() {
  pid_t child_pid;
  uint64_t i, n;
  n = 100;
  pid_t* children = (pid_t*)malloc(sizeof(pid_t) * n);
  i = 0;
  while (i < n) {
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
        children[i] = child_pid;
        break;
    }
    ++i;
  }
  i = 0;
  while (i < n) {
    if (waitpid(children[i], NULL, 0) == -1) { perror("waitpid"); return -1; }
    ++i;
  }
  free(children);
  return 0;
}
