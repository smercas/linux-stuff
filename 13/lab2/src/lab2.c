#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/wait.h>

#define NUMBER_OF_SWINGS 100

void handler_sigusr1() {
  return;
}

int32_t main() {
  pid_t pid;
  int32_t i;
  sigset_t blocker, old_mask, unblocker;

  if (sigemptyset(&blocker) == -1) { perror("sigemptyset"); return -1; }
	if (sigaddset(&blocker, SIGUSR1) == -1) { perror("sigaddset"); return -1; }
  if (sigprocmask(SIG_BLOCK, &blocker, &old_mask) == -1) { perror("sigprocmask"); return -1; }

  if (sigfillset(&unblocker) == -1) { perror("sigfillset"); return -1; }
	if (sigdelset(&unblocker, SIGUSR1) == -1) { perror("sigdelset"); return -1; }

  if (signal(SIGUSR1, handler_sigusr1) == SIG_ERR) { perror("signal"); return -1; }

  switch (pid = fork()) {
    case -1:
      perror("fork");
      return -1;
      break;
    case  0:  //child process
      for (i = 0; i < NUMBER_OF_SWINGS; ++i) {
        sigsuspend(&unblocker);
        printf("ho, ");
        fflush(stdout);
        kill(getppid(), SIGUSR1);
      }
      return 0;
      break;
    default:  //parent process
      for (i = 0; i < NUMBER_OF_SWINGS; ++i) {
        printf("heigh-");
        fflush(stdout);
        kill(pid, SIGUSR1);
        sigsuspend(&unblocker);
      }
      break;
  }
  if (waitpid(pid, NULL, 0) == -1) { perror("wait"); return -1; }
  if (sigprocmask(SIG_SETMASK, &old_mask, NULL) == -1) { perror("sigprocmask"); return -1; }
  // is this necessary?

  printf("\nThe end.\n");
	return 0;
}