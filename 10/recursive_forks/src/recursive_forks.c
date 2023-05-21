#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdint.h>

int32_t rec_children(uint64_t n, uint64_t k) {
  if (k > 0) {
    pid_t child_pid;
    uint64_t i;
    pid_t* children = (pid_t*)malloc(sizeof(pid_t) * n);
    i = 0;
    while (i < n) {
      switch (child_pid = fork()) {
        case -1:  //error
          perror("fork");
          return -1;
          break;
        case 0:   //child
          if (rec_children(n, k - 1) < 0) { fprintf(stderr, "rec_children: failed for some reason\n"); return -1; }
          return 0;
          break;
        default:  //parent
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
  }
  return 0;
}

int32_t main() {
  uint64_t n, k; //k levels, n children
  n = 5; k = 5;
  if (rec_children(n, k) < 0) { fprintf(stderr, "rec_children: failed for some reason\n"); return -1; }
  return 0;
}
