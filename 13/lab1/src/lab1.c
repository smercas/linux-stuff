#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <fcntl.h>
#include <stdint.h>

void handle_sigint() {
	printf("\nProcess has been terminated.\n");
	exit(0);
}

void handle_sigusr1() {
  printf("\n");
	system("uname -a");
} //kill -10 (pid)

void handle_sigalrm() {
	int32_t fd, read_code;
	char ch;

	if ((fd = open("remember.txt", O_RDONLY)) == -1) { perror("open"); exit(1); }

	printf("Remember:\n");
  while ((read_code = read(fd, &ch, sizeof(char))) > 0) {
    printf("%c", ch);
    fflush(stdout);
  }
  if (read_code == -1) { perror("read"); exit(-1); }
	printf("\n");
	if (close(fd) == -1) { perror("close"); exit(-1); }
}

int32_t main() {
	if (signal(SIGINT, handle_sigint) == SIG_ERR) { perror("signal"); return -1; }
	if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) { perror("signal"); return -1; }
	if (signal(SIGALRM, handle_sigalrm) == SIG_ERR) { perror("signal"); return -1; }
    
	while (1) {
		alarm(10);
		pause();
	}

	if (signal(SIGINT, SIG_DFL) == SIG_ERR) { perror("signal"); return -1; }
	if (signal(SIGUSR1, SIG_DFL) == SIG_ERR) { perror("signal"); return -1; }
	if (signal(SIGALRM, SIG_DFL) == SIG_ERR) { perror("signal"); return -1; }

  printf("The end.\n");
	return 0;
}
