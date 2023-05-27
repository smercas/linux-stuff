#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int32_t main(int32_t argc, char** argv) {
  char* map_address;
  int32_t fd;
  struct stat s;
  off_t offset, page_alligned_offset;
  int64_t length, adjusted_length;
  int64_t write_code;
  int64_t page_size;
  if (argc - 1 != 2 && argc - 1 != 3) { fprintf(stderr, "Usage: io_file offset [length]\n"); return -1; }
  if ((page_size = sysconf(_SC_PAGE_SIZE)) == -1) { perror("sysconf"); return -1; }
  if ((fd = open(argv[1], O_RDWR), 0600) == -1) { perror("open"); return -1; };
  if (fstat(fd, &s) == -1) { perror("fstat"); return -1; }
  if (sscanf(argv[2], "%ld", &offset) != 1) { fprintf(stderr, "Invalid offset: %s\n", argv[2]); return -1; }
  if (offset >= s.st_size) { fprintf(stderr, "File offset(%ld) is past the end of the file(%ld).\n", offset, s.st_size); return -1; }
  page_alligned_offset = offset / page_size * page_size;
  if (argc - 1 == 2) {
    length = s.st_size - offset;
  }
  else {
    if (sscanf(argv[3], "%lu", &length) != 1) { fprintf(stderr, "Invalid length: %s\n", argv[3]); return -1; }
    if (offset + length > s.st_size) {
      length = s.st_size - offset;
    }
  }
  adjusted_length = length + offset - page_alligned_offset;
  if ((map_address = mmap(NULL,
                          adjusted_length,
                          PROT_READ | PROT_WRITE,
                          MAP_SHARED,
                          fd,
                          page_alligned_offset)) == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(fd) == -1) { perror("close"); return -1; }
  write_code = write(STDOUT_FILENO, map_address + offset - page_alligned_offset, length);
  if (write_code == -1) { perror("write"); return -1; }
  else if (write_code != length) { perror("partial write"); return -1; }
  int64_t i = offset - page_alligned_offset;
  while (i < adjusted_length) {
    if (map_address[i] >= 'a' && map_address[i] < 'z') {
      ++map_address[i];
    }
    ++i;
  }
  write_code = write(STDOUT_FILENO, map_address + offset - page_alligned_offset, length);
  if (write_code == -1) { perror("write"); return -1; }
  else if (write_code != length) { perror("partial write"); return -1; }
  if (msync(map_address, adjusted_length, MS_SYNC | MS_INVALIDATE)) { perror("msync"); return -1; }
  if (munmap(map_address, adjusted_length) == -1) { perror("munmap"); return -1; }
  return 0;
}
