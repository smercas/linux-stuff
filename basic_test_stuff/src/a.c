#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdbool.h>

#define helper_messages true

int32_t main(int32_t argc, char** argv) {
  if (argc - 1 != 1) { fprintf(stderr, "Usage: input_file_name\n"); return -1; }
  pid_t pid1, pid2;
  int32_t p1[2];
  if (pipe(p1) == -1) { perror("pipe"); return -1; }
  if (mkfifo("fifo_P2toP1", 0600) == -1 && errno != EEXIST) { perror("mkfifo"); return -1; }
  switch (pid1 = fork()) {
    case -1: {
      perror("first fork");
      return -1;
      break;
    }
    case  0: {  //first child process
      int32_t fd;
      //close unused pipes
      if (close(p1[0]) == -1) { perror("p1[0] close"); return -1; }
      if (close(p1[1]) == -1) { perror("p1[1] close"); return -1; }
      //redirect input to fifo_P2toP1
      if ((fd = open("fifo_P2toP1", O_RDONLY, 0400)) == -1) { perror("fifo_P2toP1 input open"); return -1; }
      if (dup2(fd, STDIN_FILENO) == -1) { perror("P1 input dup2"); return -1; }
      if (close(fd) == -1) { perror("fifo_P2toP1 input close"); return -1; }
      //output handled by prg_p1
      //exec stuff
      if (execl("bin/prg_p1", "prg_p1", NULL) == -1) { perror("prg_p1 execl"); return -1; }
      return 0;
      break;
    }
    default: {
      switch (pid2 = fork()) {
        case -1: {
          perror("second fork");
          return -1;
          break;
        }
        case  0: {  //second child process
          int32_t fd;
          //close unused pipes
          if (close(p1[1]) == -1) { perror("p1[1] close"); return -1; }
          //redirect input to p1[0]
          if (dup2(p1[0], STDIN_FILENO) == -1) { perror("P2 input dup2"); return -1; }
          if (close(p1[0]) == -1) { perror("p1[1] close"); return -1; }
          //redirect output to fifo
          if ((fd = open("fifo_P2toP1", O_WRONLY, 0200)) == -1) { perror("fifo_P2toP1 output open"); return -1; }
          if (dup2(fd, STDOUT_FILENO) == -1) { perror("P2 output dup2"); return -1; }
          if (close(fd) == -1) { perror("fifo_P2toP1 output close"); return -1; }
          //exec stuff
          if (execl("bin/prg_p2", "prg_p2", NULL) == -1) { perror("prg_p2 execl"); return -1; }
          break;
        }
        default: {  //parent process
          int32_t fd;
          int64_t number;
          int64_t i, j, count, max, min;
          char* input_map_address;
          int64_t* output_map_address;
          struct stat s;
          const char* recieved_file = "comm_P1toP0.bin";
          //close unused pipes
          if (close(p1[0]) == -1) { perror("p1[0] close"); return -1; }
          //map file
          if ((fd = open(argv[1], O_RDWR, 0600)) == -1) { perror("open"); return -1; }
          if (fstat(fd, &s) == -1) { perror("fstat"); return -1; }
          if ((input_map_address = mmap(NULL,
                                        s.st_size,
                                        PROT_READ | PROT_WRITE,
                                        MAP_PRIVATE,
                                        fd,
                                        0)) == MAP_FAILED) { perror("mmap"); return -1; }
          if (close(fd) == -1) { perror("close"); return -1; }
          i = 0;
          j = 0;
          count = 0;
          while (i + j <= s.st_size) {
            if (i + j == s.st_size || input_map_address[i + j] == ' ') {
              i = i + j + 1;
              if (j != 0) {
                ++count;
                j = 0;
              }
            } 
            else {
              ++j;
            }
          }
          //truncating "comm_P1toP0.bin" cause I don't know where else i could truncate it
          if ((fd = open(recieved_file, O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) { perror("trunc open"); return -1; }
          if (ftruncate(fd, (count + 1) / 2 * sizeof(int64_t)) == -1) { perror("ftruncate"); return -1; }
          if (close(fd) == -1) { perror("truncate close"); return -1; }
          //read numbers form the mapped file and write them into p1[1]
          // printf("count = %ld\n", count);
          // if (write(p1[1], &count, sizeof(int64_t)) != sizeof(int64_t)) { perror("count write"); return -1; }
          i = 0;
          j = 0;
          while (i + j <= s.st_size) {
            if (i + j == s.st_size || input_map_address[i + j] == ' ') {
              if (j != 0) {
                input_map_address[i + j] = '\0';
                if (sscanf(input_map_address + i, "%ld", &number) != 1) { fprintf(stderr, "Invalid number: %s\n", input_map_address + i); return -1; }
                // printf("number: %ld\n", number);
                if (write(p1[1], &number, sizeof(int64_t)) != sizeof(int64_t)) { fprintf(stderr, "numbers[%ld] write: ", i); perror(""); return -1; }
                input_map_address[i + j] = ' ';
              }
              i = i + j + 1;
              if (j != 0) {
                j = 0;
              }
            }
            else {
              ++j;
            }
          }
          if (close(p1[1]) == -1) { perror("p1[1] close"); return -1; }
          if (munmap(input_map_address, s.st_size) == -1) { perror("munmap"); return -1; }
          if (waitpid(pid1, NULL, 0) == -1) { perror("first waitpid"); return -1; }
          if (waitpid(pid2, NULL, 0) == -1) { perror("second waitpid"); return -1; }  //important (could also use a semaphore here)
          //map the output that was recieved form P1
          if ((fd = open(recieved_file, O_RDONLY, 0600)) == -1) { perror("shm_open"); return -1; }
          if (fstat(fd, &s) == -1) { perror("fstat"); return -1; }
          if ((output_map_address = mmap(NULL,
                                         s.st_size,
                                         PROT_READ,
                                         MAP_PRIVATE,
                                         fd,
                                         0)) == MAP_FAILED) { perror("mmap"); return -1; }
          if (close(fd) == -1) { perror("close"); return -1; }
          //calculate the max and min
          if (s.st_size < (int64_t)sizeof(int64_t)) { fprintf(stderr, "cannot determine the max and min of the numbers that've been recieved from P2\n"); return -1; }
          // i = 0;
          // while (i < s.st_size / (int64_t)sizeof(int64_t)) {
          //   printf("%ld ", output_map_address[i]);
          //   ++i;
          // }
          // printf("\n");
          max = output_map_address[0];
          min = output_map_address[0];
          i = 1;
          while (i < s.st_size / (int64_t)sizeof(int64_t)) {
            if (max < output_map_address[i]) {
              max = output_map_address[i];
            }
            if (min > output_map_address[i]) {
              min = output_map_address[i];
            }
            ++i;
          }
          //print the max and min to stdout
          if (printf("max: %ld\nmin: %ld\n", max, min) < 0) { perror("printf"); return -1; }
          if (munmap(input_map_address, s.st_size) == -1) { perror("munmap"); return -1; }
          if (unlink("fifo_P2toP1") == -1) { perror("fifo_P2toP1 unlink"); return -1; }
          if (unlink("comm_P1toP0.bin") == -1) { perror("comm_P1toP0 unlink"); return -1; }
          return 0;
          break;
        }
      }
      break;
    }
  }
}
