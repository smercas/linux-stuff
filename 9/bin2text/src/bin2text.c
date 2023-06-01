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
  uint32_t i;
  int32_t input_bin_fd, output_text_fd;
  struct stat isb;
  int64_t* input_bin_map_address;
  char number_buffer[128];
  if (argc - 1 != 2) { fprintf(stderr, "Usage: input_file_name output_file_name\n"); return -1; }
  if ((input_bin_fd = open(argv[1], O_RDONLY, 0600)) == -1) { perror("input open"); return -1; }
  if (fstat(input_bin_fd, &isb) == -1) { perror("input fstat"); return -1; }
  if (isb.st_size == 0) { fprintf(stderr, "Recieved file (%s) is empty\n", argv[1]); return -1; }
  if ((input_bin_map_address = mmap(NULL,
                                    isb.st_size,
                                    PROT_READ,
                                    MAP_PRIVATE,
                                    input_bin_fd,
                                    0)) == MAP_FAILED) { perror("input mmap"); return -1; }
  if (close(input_bin_fd) == -1) { perror("input close"); return -1; }
  if ((output_text_fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0600)) == -1) { perror("output open"); return -1; }
  i = 0;
  while (i < isb.st_size / sizeof(int64_t)) {
    if (sprintf(number_buffer, "%ld ", input_bin_map_address[i]) < 0) { fprintf(stderr, "sprintf: "); perror(""); return -1; }
    if (write(output_text_fd, number_buffer, strlen(number_buffer)) == -1) { perror("output write"); return -1; }
    ++i;
  }
  if (i != 0) {
    if (lseek(output_text_fd, -1, SEEK_CUR) == -1) { perror("output lseek"); return -1; }
    if (write(output_text_fd, "\n", strlen("\n")) == -1) { perror("output endline write"); return -1; }
  }
  if (munmap(input_bin_map_address, isb.st_size) == -1) { perror("input munmap"); return -1; }
  if (close(output_text_fd) == -1) { perror("input close"); return -1; }
  return -1;
}
