#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>

int32_t main() {
  printf("The current process has the following PID:\t%d.\nIts parent has the following PID:\t\t%d.\n\n", getpid(), getppid());

  printf("The real owner of the current process is:\tUID=%d GID=%d.\n", getuid(), getgid());
  printf("The effective owner of the current process is:\tUID=%d GID=%d.\n\n", geteuid(), getegid());

  printf("3 seconds of eepy behaviour...\n\n");
  sleep(3);
  printf("We up now!\n");
  return 0;
}