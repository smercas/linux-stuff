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
#define BUF_SIZE 4096

char input_file_name[PATH_MAX];
char output_file_name[PATH_MAX];
int32_t input_fd;
int32_t output_fd;
int32_t io_fd;
char mode;
char pair[2];

int32_t process_file(int32_t input_descriptor, int32_t output_descriptor, char p[2]) {
  ssize_t bytes_in;
  ssize_t bytes_out;
  char buffer[BUF_SIZE];
  int64_t i;

  while ((bytes_in  = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    i = 0;
    while (i < bytes_in) {
      if (buffer[i] == p[0]) {
        buffer[i] = p[1];
      }
      ++i;
    }
    if (input_descriptor == output_descriptor) {
      if (lseek(output_descriptor, -bytes_in, SEEK_CUR) == -1) { perror("lseek"); return -1; }
    }
    bytes_out = write(output_descriptor, buffer, bytes_in);
    if (bytes_out != bytes_in) { perror("bad write"); return -1; }
    if (bytes_out == -1) { perror("write"); return -1; }
  }
  if (bytes_in == -1) { perror("read"); return -1; }
  return 0;
}

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
      printf("%s __ %s\n\n", argv[1], argv[2]);
          if ((input_fd = open(argv[1], O_RDONLY, 0600)) == -1) { perror("input open"); return -1; }
          if ((output_fd = open(argv[2], O_WRONLY | O_TRUNC, 0600)) == -1) { perror("output open"); return -1; }
          //output file exists but it was overwritten
        }
      }
    }
    else {
      if ((input_fd = open(argv[1], O_RDONLY, 0400)) == -1) { perror("created output input open"); return -1; }
      if ((output_fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL | O_TRUNC, 0600)) == -1) { perror("output create"); return -1; }
      //output file does not exist
    }
  }
  pair[0] = argv[argc - 2][0];
  pair[1] = argv[argc - 1][0];
  if (mode == 0) {
    if (process_file(input_fd, output_fd, pair) < 0) { fprintf(stderr, "failed to process files"); return -1; }
    if (close(input_fd) == -1) { perror("input file close"); return -1; }
    if (close(output_fd) == -1) { perror("input file close"); return -1; }
  }
  else if (mode == 1) {
    if (process_file(io_fd, io_fd, pair) < 0) { fprintf(stderr, "failed to process file"); return -1; }
    if (close(io_fd) == -1) { perror("input file close"); return -1; }
  }
  return 0;
}
