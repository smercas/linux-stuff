#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>

int32_t main()
{
  sigset_t base_mask, waiting_mask;
  int32_t i;
  if (sigemptyset(&base_mask) == -1) { perror("sigemptyset"); return -1; }
  if (sigaddset(&base_mask, SIGINT) == -1) { perror("sigaddset"); return -1; }
  if (sigaddset(&base_mask, SIGTSTP) == -1) { perror("sigemptyset"); return -1; }
  if (sigprocmask(SIG_SETMASK, &base_mask, NULL) == -1) { perror("sigprocmask"); return -1; }

  for (i = 1; i <= 10; ++i) {
    sleep(1);
    printf("Time: %d seconds\n", i);
  }

  if (sigpending(&waiting_mask) == -1) { perror("sigpending"); return -1; }

  switch (sigismember(&waiting_mask, SIGINT)) {
  case -1:
    perror("sigismember");
    return -1;
    break;
  case  1:
    printf("The user tried to stop the process using CTRL+C.\n");
    break;
  default:
    break;
  }
  switch (sigismember(&waiting_mask, SIGTSTP)) {
  case -1:
    perror("sigismember");
    return -1;
    break;
  case  1:
    printf("The user tried to stop the process using CTRL+Z.\n");
    break;
  default:
    break;
  }

  if (sigprocmask(SIG_UNBLOCK, &base_mask, NULL) == -1) { perror("sigprocmask"); return -1; }
  for (i = 1; i <= 10; ++i) {
    sleep(1);
    printf("Time: %d seconds\n", i);
  }

  printf("The end.\n");
  return 0;
}
