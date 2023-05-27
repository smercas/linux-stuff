#include <stdio.h>
#include <signal.h>
#include <stdint.h>

int32_t main() {
  sigset_t mask;

  if (sigfillset(&mask) == -1) { perror("sigfillseSt"); return -1; }
  if (sigdelset(&mask, SIGQUIT) == -1) { perror("sigdelset"); return -1; }

  printf("Execution will be suspended until the user uses CTRL+\\.\n");
  sigsuspend(&mask);

  printf("The end.\n");
  return 0;
}