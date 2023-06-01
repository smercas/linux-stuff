#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/limits.h>
#include <stdint.h>

int32_t main(int32_t argc, char** argv) {
  int64_t i, j, count;
  int32_t input_text_fd, output_bin_fd;
  struct stat isb;
  char* input_text_map_address;
  char separator;
  int64_t* output_bin_map_address;
  if (argc - 1 != 2) { fprintf(stderr, "Usage: input_file_name output_file_name\n"); return -1; }
  if ((input_text_fd = open(argv[1], O_RDONLY), 0600) == -1) { perror("input open"); return -1; }
  if (fstat(input_text_fd, &isb) == -1) { perror("input fstat"); return -1; }
  if ((input_text_map_address = mmap(NULL,
                                     isb.st_size,
                                     PROT_READ | PROT_WRITE,
                                     MAP_PRIVATE,
                                     input_text_fd,
                                     0)) == MAP_FAILED) { perror("input mmap"); return -1; }
  if (close(input_text_fd) == -1) { perror("input close"); return -1; }
  i = 0;
  j = 0;
  count = 0;
  while (i + j <= isb.st_size) {
    if (i + j == isb.st_size || strchr(" \n", input_text_map_address[i + j])) {
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
  if (count == 0) { fprintf(stderr, "Recieved file (%s) does not have any numbers\n", argv[1]); return -1; }
  if ((output_bin_fd = open(argv[2], O_RDWR | O_CREAT, 0600)) == -1) { perror("output open"); return -1; }
  if (ftruncate(output_bin_fd, count * sizeof(int64_t)) == -1) { perror("outputftruncate"); return -1; }
  if ((output_bin_map_address = mmap(NULL,
                                     count * sizeof(int64_t),
                                     PROT_READ | PROT_WRITE,
                                     MAP_SHARED,
                                     output_bin_fd,
                                     0)) == MAP_FAILED) { perror("output mmap"); return -1; }
  if (close(output_bin_fd) == -1) { perror("output close"); return -1; }
  i = 0;
  j = 0;
  count = 0;
  while (i + j <= isb.st_size) {
    if (i + j == isb.st_size || strchr(" \n", input_text_map_address[i + j])) {
      if (j != 0) {
        separator = input_text_map_address[i + j];
        input_text_map_address[i + j] = '\0';
        if (sscanf(input_text_map_address + i, "%ld", &output_bin_map_address[count]) != 1) { fprintf(stderr, "Invalid number: %s\n", input_text_map_address + i); return -1; }
        // printf("output_bin_map_address[%ld]: %ld\n", count, output_bin_map_address[count]);
        input_text_map_address[i + j] = separator;
        ++count;
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
  if (msync(output_bin_map_address, count * sizeof(int64_t), MS_SYNC | MS_INVALIDATE) == -1) { perror("output sync"); return -1; }
  if (munmap(input_text_map_address, isb.st_size) == -1) { perror("input munmap"); return -1; }
  if (munmap(output_bin_map_address, count * sizeof(int64_t)) == -1) { perror("output munmap"); return -1; }
  return -1;
}
