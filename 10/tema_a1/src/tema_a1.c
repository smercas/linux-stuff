#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int32_t grandchildren_creation(int32_t k) {
  int32_t i;
  pid_t pid;
  pid_t child[2];
  int32_t code_child[2];
  for (i = 0; i < 2; ++i) {
    switch (pid = fork()) {
      case -1:
        { perror("fork"); return -1; }
        break;
      case  0:
        printf("\t\t(3, %d) PID=%d PPID=%d\n", k * 2 + i + 1, getpid(), getppid());
        sleep(1);
        return i;
      default:
        child[i] = pid;
        break;
    }
  }
  for (i = 0; i < 2; ++i) {
    if (waitpid(child[i], &code_child[i], 0) == -1) { perror("wait"); return -1; }
  }
  printf("\t(2, %d): PID=%d, PPID=%d\n", k + 1, getpid(), getppid());
  for (i = 0; i < 2; ++i) {
    printf("\tchild %d: PID=%d, termination_code=%d\n", i + 1, child[i], code_child[i]);
  }
  return 0;
}

int32_t main() {
  int32_t i;
  pid_t pid;
  pid_t child[5];
  int32_t code_child[5];
  printf("Initial process: PID=%d PPID=%d.\n", getpid(), getppid());

  /* Bucla de producere a primelor 5 procese fii. */
  for (i = 0; i < 5; ++i) {
    switch (pid = fork()) {
      case -1:
        { perror("fork"); return -1; }
        break;
      case  0:
        if (grandchildren_creation(i) < 0) { fprintf(stderr, "grandchildren_creation failed for some reason"); return -1; }
        sleep (1);
        return i;
      default:
        child[i] = pid;
        break;
    }
  }
  //  now we wait
  for (i = 0; i < 5; ++i) {
    if (wait(&code_child[i]) == -1) { perror("wait"); return -1; }
  }
  printf("(1, 1): PID=%d PPID=%d\n", getpid(), getppid());
  for (i = 0; i < 5; ++i) {
    printf("child %d: %d termination_code=%d\n", i + 1, child[i], code_child[i]);
  }
  return 0;
}
