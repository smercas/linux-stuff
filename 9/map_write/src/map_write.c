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
  int64_t length;
  int64_t write_code;
  int64_t page_size;
  char* text_de_scris = "Acest mesaj se va scrie in fisierul mapat in memorie.\n";
  
  if (argc - 1 != 1) { fprintf(stderr, "output_file\n"); return -1; }
  if ((page_size = sysconf(_SC_PAGE_SIZE)) == -1) { perror("sysconf"); return -1; }
  if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC), 0600) == -1) { perror("open"); return -1; };
  length = strlen(text_de_scris);
  if (ftruncate(fd, length) == -1) { perror("ftrunacte"); return -1; }
  map_address = mmap(NULL,
                     length,
                     PROT_READ | PROT_WRITE,
                     MAP_SHARED,
                     fd,
                     0);
  if (map_address == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(fd) == -1) { perror("close"); return -1; }
  int64_t i = 0;
  while (i < length) {
    map_address[i] = text_de_scris[i];
    ++i;
  }
  write_code = write(STDOUT_FILENO, map_address, length);
  if (write_code == -1) { perror("write"); return -1; }
  else if (write_code != length) { perror("partial write"); return -1; }
  if (msync(map_address, length, MS_SYNC)) { perror("msync"); return -1; }
  if (munmap(map_address, length) == -1) { perror("munmap"); return -1; }
  return 0;
}
