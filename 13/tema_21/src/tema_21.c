#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdbool.h>
#include <fcntl.h>

char* file_name;
uint64_t count, N;
bool written;
int32_t fd;

void handle_sigusr1() {
	printf("Child process: recieved SIGUSR1.\n");
  char* map_address;
  int64_t write_code;
  struct stat s;
  if ((fd = open(file_name, O_RDONLY, 0400)) == -1) { perror("read open"); exit(-1); }
  if (fstat(fd, &s) == -1) { perror("stat"); exit(-1); }
  map_address = mmap(NULL, 
                     s.st_size,
                     PROT_READ,
                     MAP_PRIVATE,
                     fd,
                     0);
  if (map_address == MAP_FAILED) { perror("mmap"); exit(-1); }
  if (close(fd) == -1) { perror("close"); exit(-1); }
  if ((write_code = write(STDOUT_FILENO, map_address, s.st_size)) == -1) { perror("write"); exit(-1); }
  else if (write_code != s.st_size) { perror("partial write"); exit(-1); }
  if (munmap(map_address, s.st_size) == -1) { perror("munmap"); exit(-1); }
  printf("Child process: end of process execution.\n");
  exit(0);
}

void handle_sigchild() {
  printf("Parent process: end of process execution.\n");
  exit(0);
}

void handle_alarm() {
  if (!written) {
    ++count;
    if (count >= 3) {
      exit (-1);
    }
    printf("Parent process: write something.\n");
    alarm(N);
  }
  else {
    return;
  }
}

int32_t main(int32_t argc, char** argv) {
  if (argc - 1 != 2) {
    fprintf(stderr, "usage: %s <output_file> <N>", argv[0]);
    return -1;
  }
  pid_t pid;
  written = false;
  count = 0;
  file_name = argv[1];
  N = strtoull(argv[2], NULL, 10);

  switch (pid = fork()) {
    case -1:
      perror("fork");
      return -1;
      break;
    case  0:  //child process
      if (signal(SIGUSR1, handle_sigusr1) == SIG_ERR) { perror("signal"); return -1; }
      for(;;)
        sleep(1);
      break;
    default:  //parent process
      //copy stdin into argv[1]
      if ((fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) { perror("write and truncate open"); return -1; }
      //if nothing is written in 3 * N seconds then end the process
      //also print a warning every N seconds
      if (signal(SIGALRM, handle_alarm) == SIG_ERR) { perror("signal"); return -1; }
      alarm(N);
      char ch;
      //write from stdin until EOF
      while((ch = getc(stdin)) != EOF) {
        written = true;
        if (write(fd, &ch, sizeof(char)) == -1) { perror("write"); return -1; }
      }
      if (close(fd) == -1) { perror("close"); return -1; }
      if (signal(SIGCHLD, handle_sigchild) == SIG_ERR) { perror("signal"); return -1; }
      kill(pid, SIGUSR1);
      if (signal(SIGALRM, SIG_DFL) == SIG_ERR) { perror("signal"); return -1; }
      if (signal(SIGCHLD, SIG_DFL) == SIG_ERR) { perror("signal"); return -1; }
      for(;;)
        sleep(1);
      break;
  }
  return 0;
}
