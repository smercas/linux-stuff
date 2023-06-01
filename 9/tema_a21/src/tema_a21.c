#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <linux/limits.h>
#include <libgen.h>
#include <sys/mman.h>
#define BUF_SIZE 4096
int32_t input_fd;
int32_t output_fd;
int32_t io_fd;
char mode;
char pair[2];

int32_t main(int32_t argc, char** argv) {
  if (argc - 1 != 3 && argc - 1 != 4) { fprintf(stderr, "Usage:\tinput_file output_file original_char replacement\n\t    OR\n\tio_file original_char replacement\n"); return -1; }
  else if (argc - 1 == 3) {
    mode = 1;
    if ((io_fd = open(argv[1], O_RDWR, 0600)) == -1) { perror("io open"); return -1; }
    //only one file anme recieved
  }
  else {
    if (access(argv[2], F_OK) == 0) {
      struct stat sb_i, sb_o;
      if (stat(argv[1], &sb_i) == -1) { perror("input file stat"); return -1; }
      if (stat(argv[2], &sb_o) == -1) { perror("output file stat"); return -1; }
      if (sb_i.st_dev == sb_o.st_dev && sb_i.st_ino == sb_o.st_ino) {
        mode = 1;
        if ((io_fd = open(argv[1], O_RDWR, 0600)) == -1) { perror("io open"); return -1; }
        //recieved two file names but they point towards the same name
      }
      else {
        char answer = 'x';
        while (!strchr("yn", answer)) {
          printf("The output file already exists, do you want to overwrite it? (y/n)\n");
          scanf("%c", &answer);
        }
        if (answer == 'n') {
          return 1;
          //output file exists, refused to overwrite it
        }
        else {
          mode = 0;
          if ((input_fd = open(argv[1], O_RDONLY, 0600)) == -1) { perror("input open"); return -1; }
          if ((output_fd = open(argv[2], O_RDWR | O_TRUNC, 0600)) == -1) { perror("output open"); return -1; }
          //output file exists but it was overwritten
        }
      }
    }
    else {
      if ((input_fd = open(argv[1], O_RDONLY, 0600)) == -1) { perror("created output input open"); return -1; }
      if ((output_fd = open(argv[2], O_RDWR | O_CREAT | O_EXCL | O_TRUNC, 0600)) == -1) { perror("output create"); return -1; }
      //output file does not exist
    }
  }
  pair[0] = argv[argc - 2][0];
  pair[1] = argv[argc - 1][0];
  if (mode == 0) {
    char* i_map_address;
    char* o_map_address;
    struct stat s;
    int64_t i;
    if (fstat(input_fd, &s) == -1) { perror("fstat"); return -1; }
    if ((i_map_address = mmap(NULL,
                              s.st_size,
                              PROT_READ,
                              MAP_PRIVATE,
                              input_fd,
                              0)) == MAP_FAILED) { perror("mmap"); return -1; }
    if (close(input_fd) == -1) { perror("input file close"); return -1; }
    if (ftruncate(output_fd, s.st_size) == -1) { perror("ftruncate"); return -1; }
    if ((o_map_address = mmap(NULL,
                              s.st_size,
                              PROT_WRITE,
                              MAP_SHARED,
                              output_fd,
                              0)) == MAP_FAILED) { perror("mmap"); return -1; }
    if (close(output_fd) == -1) { perror("output file close"); return -1; }
    i = 0;
    while (i < s.st_size) {
      if (i_map_address[i] == pair[0]) {
        o_map_address[i] = pair[1];
      }
      else {
        o_map_address[i] = i_map_address[i];
      }
      ++i;
    }
    if (msync(o_map_address, s.st_size, MS_SYNC | MS_INVALIDATE) == -1) { perror("msync"); return -1; }
    if (munmap(i_map_address, s.st_size) == -1) { perror("munmap"); return -1; }
    if (munmap(o_map_address, s.st_size) == -1) { perror("munmap"); return -1; }
  }
  else if (mode == 1) {
    char* map_address;
    struct stat s;
    int64_t i;
    if (fstat(io_fd, &s) == -1) { perror("fstat"); return -1; }
    if ((map_address = mmap(NULL,
                            s.st_size,
                            PROT_READ | PROT_WRITE,
                            MAP_SHARED,
                            io_fd,
                            0)) == MAP_FAILED) { perror("mmap"); return -1; }
    if (close(io_fd) == -1) { perror("io file close"); return -1; }
    i = 0;
    while (i < s.st_size) {
      if (map_address[i] == pair[0]) {
        map_address[i] = pair[1];
      }
      ++i;
    }
    if (msync(map_address, s.st_size, MS_SYNC | MS_INVALIDATE) == -1) { perror("msync"); return -1; }
    if (munmap(map_address, s.st_size) == -1) { perror("munmap"); return -1; }
  }
  return 0;
}
