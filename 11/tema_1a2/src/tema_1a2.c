#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pwd.h>

int32_t main(int32_t argc, char** argv) {
  pid_t pid;
  int32_t term_code;
  if (argc - 1 != 0) { fprintf(stderr, "usage:\n"); return -1; }
  switch (pid = fork()) {
    case -1:
      perror("fork");
      return -1;
      break;
    case 0:
      char* args[] = {
        "find",
        getpwuid(getuid())->pw_dir,
        "-exec",
        "stat",
        "--format=%a %n",
        "{}",
        ";",
        NULL
      };
      execv("/usr/bin/find", args);
      perror("execv");
      exit(3);
      break;
    default:
      break;
  }
  if (waitpid(pid, &term_code, 0) == -1) { perror("waitpid"); return -1; }
  if (WIFEXITED(term_code)) {
    switch (WEXITSTATUS(term_code)) {
      case 3:
        printf("The find command could not be executed (execution failed).\n");
        break;
      case 0:
        printf("The find command has been successfully executed!\n");
        break;
      default:
        printf("The find command has been executed, but it failed with the following code: %d.\n", WEXITSTATUS(term_code));
        break;
    }
  }
  else {
    printf("The find command was forced to stop by the following signal: %d.\n", WTERMSIG(term_code));
  }
  return 0;
}
