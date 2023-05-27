#include <stdio.h>
#include <stdint.h>

int32_t main() {
  pid_t pid1, pid2;
  switch (pid1 = fork()) {
    case -1:
      perror("first fork");
      return -1;
      break;
    case  0:  //first child process
      break;
    default:
      switch(pid2 = fork()) {
        case -1:
          perror("second fork");
          return -1;
          break;
        case  0:  //second child process
          break;
        default:  //parent process
          break;
      }
      break;
  }
}