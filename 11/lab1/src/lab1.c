#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdint.h>

int32_t main(int argc, char** argv) {
  pid_t pid;
  int32_t term_code;
  char* dirname = (argc > 1 ? argv[1] : ".");
  switch (pid = fork()) {
    case -1:
      perror("fork");
      return -1;
      break;
    case 0:
      execl("/usr/bin/ls", "ls", "-l", "-i", dirname, NULL);
      perror("execl");
      return -1;
      break;
    default:
      break;
  }
  if (wait(&term_code) == -1) { perror("wait"); return -1; }
  if (WIFEXITED(term_code)) {
    switch (WEXITSTATUS(term_code)) {
      case 3:
        printf("The ls command could not be executed (execution failed).\n");
        break;
      case 0:
        printf("The ls command has been successfully executed!\n");
        break;
      default:
        printf("The ls command has been executed, but it failed with the following code: %d.\n", WEXITSTATUS(term_code));
        break;
    }
  }
  else {
    printf("The ls command was forced to stop by the following signal: %d.\n", WTERMSIG(term_code));
  }
  return 0;
}
