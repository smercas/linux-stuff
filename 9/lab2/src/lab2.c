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
  int32_t nr, fd;
  struct stat sb;
  off_t file_length;
  char* map_address;
  int32_t* p_nr;
  char file_name[PATH_MAX];
  if (argc - 1 == 0) {
    printf("Insert the input file name here: ");
    scanf("%s", file_name);
  }
  else if (argc - 1 == 1) {
    strcpy(file_name, argv[1]);
  }
  else { fprintf(stderr, "Usage: [input_file_name]\n"); return -1; }
  if ((fd = open(file_name,O_RDONLY)) == -1) { perror("open"); return -1; }
  if (fstat(fd, &sb) == -1) { perror("fstat"); return -1; }
  file_length = sb.st_size;
  if ((map_address = mmap(NULL,
                          file_length,
                          PROT_READ,
                          MAP_PRIVATE,
                          fd,
                          0)) == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(fd) == -1) { perror("close"); return -1; }
  printf("The recieved file contains the following numbers:\n");
  i = 0;
  p_nr = (int32_t*)map_address;
  while (i < file_length / sizeof(int32_t)) {
    nr = *p_nr;
    printf((i == 0 ? "%d" : ", %d"), nr);
    ++i;
    ++p_nr;
  }
  printf("\n");
  if (munmap(map_address, file_length) == -1) { perror("munmap"); return -1; }
  return -1;
}
