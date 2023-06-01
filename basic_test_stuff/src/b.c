#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

int32_t main() {
  int32_t read_code;
  int64_t number, product, i, fd;
  const char* mapped_file_name = "comm_P1toP0.bin";
  int64_t* map_address;
  struct stat s;
  //setting up the map
  if ((fd = open(mapped_file_name, O_RDWR, 0600)) == -1) { perror("shm_open"); return -1; }
  if (fstat(fd, &s) == -1) { perror("fstat"); return -1; }
  if ((map_address = mmap(NULL,
                          s.st_size,
                          PROT_WRITE,
                          MAP_SHARED,
                          fd,
                          0)) == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(fd) == -1) { perror("close"); return -1; }
  i = 0;
  while ((read_code = read(STDIN_FILENO, &number, sizeof(int64_t))) > 0) {
    // printf("number[%ld]: %ld\n", i, number);
    if (i % 2 == 0) {
      product = number;
    }
    else {
      product = product * number;
      map_address[i / 2] = product;
      // printf("\tproduct[%ld]: %ld\n", i / 2, product);
      product = 1;
    }
    ++i;
  }
  if (read_code == -1) { perror("read"); return -1; }
  if (i % 2 != 0) {
    map_address[i / 2] = product;
    // printf("\tproduct[%ld]: %ld\n", i / 2, product);
    product = 1;
    ++i;
  }
  if (msync(map_address, sizeof(int64_t) * (s.st_size / sizeof(int64_t) + 1) / 2, MS_SYNC | MS_INVALIDATE) == -1) { perror("msync"); return -1; }
  if (munmap(map_address, sizeof(int64_t) * (s.st_size / sizeof(int64_t) + 1) / 2) == -1) { perror("munmap"); return -1; }
  return 0;
}
