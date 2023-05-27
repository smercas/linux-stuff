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
  int32_t n, nr, fd, i;
  off_t file_length;
  char* map_address;
  int32_t* p_nr;
  char file_name[PATH_MAX];
  if(argc - 1 == 0) {
    printf("Insert the output file name here: ");
    scanf("%s", file_name);
  }
  else if (argc - 1 == 1) {
    strcpy(file_name, argv[1]);
  }
  else { fprintf(stderr, "Usage: [output_file_name]\n"); return -1; }
  printf("Insert the number of numbers you want to write into the file: ");
  if (scanf("%d", &n) != 1) { fprintf(stderr, "Error: incorrect format for an integer number!\n"); return -1; }
  if (n <= 0) { fprintf(stderr, "Error: you need to input a strictly positive integer number!\n"); return -1; }    
  file_length = n * sizeof(int32_t);
  if ((fd = open(file_name, O_RDWR | O_CREAT, 0600)) == -1) { perror("open"); return -1; }
  if (ftruncate(fd, file_length) == -1) { perror("ftruncate"); return -1; }
  if ((map_address = mmap(NULL,
                          file_length,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED,
                          fd,
                          0)) == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(fd) == -1) { perror("close"); return -1; }
  i = 0;
  p_nr = (int32_t*)map_address;
  while (i < n) {
    printf("Insert the %d-th number: ", i);
    if (scanf("%d", &nr) != 1) { fprintf(stderr, "Error: incorrect format for an integer number!\n"); return -1; }
    *p_nr = nr;
    ++i;
    ++p_nr;
  }
  if (msync(map_address, file_length, MS_SYNC | MS_INVALIDATE) == -1) { perror("msync"); return -1; }
  if (munmap(map_address, file_length) == -1) { perror("munmap"); return -1; }
  return 0;
}
