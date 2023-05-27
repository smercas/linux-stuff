#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdint.h>
#define NR_SEC 10

void custom_handler(int32_t sig_number) {
  printf("CTRL+%s has no effect now.\n", (sig_number == SIGINT ? "C" : "\\"));
}

int32_t main() {
  if (signal(SIGINT, custom_handler) == SIG_ERR) { perror("signal"); return -1; }
  if (signal(SIGQUIT, custom_handler) == SIG_ERR) { perror("signal"); return -1; }
  printf("CTRL+C and CTRL+\\ will be corrupted for %d seconds (or until you use the keys again)...\n", NR_SEC);
  sleep(NR_SEC);
  if (signal(SIGINT, SIG_DFL) == SIG_ERR) { perror("signal"); return -1; }
  if (signal(SIGQUIT, SIG_DFL) == SIG_ERR) { perror("signal"); return -1; }
  printf("CTRL+C and CTRL+\\ are now working again.\n");

  printf("The end.\n");
  return 0;
}
