#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>

#define NMAX 4096

int32_t main() {
  pid_t pid;
  int32_t p[2];
  if (pipe(p) == -1) { perror("pipe"); return -1; }
  switch (pid = fork()) {
    case -1:  //error
      perror("fork");
      return -1;
      break;
    case  0:  //child
      if (close(p[1]) == -1) { perror("close"); return -1; }
      //read
      if (close(p[0]) == -1) { perror("close"); return -1; }
      break;
    default:  //parent
      if (close(p[0]) == -1) { perror("close"); return -1; }
      //write
      if (close(p[1]) == -1) { perror("close"); return -1; }
      if (waitpid(pid, NULL, 0) == -1) { perror("waitpid"); return -1; }
      break;
  }
  return 0;
}
