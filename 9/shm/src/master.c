#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int32_t main() {
  const int32_t Messages_MaxSize = 100;
  const char *shm_name = "/shared_memory_obj1";
  int32_t shm_fd;
  char* shm_ptr;
  if ((shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0600)) == -1) { perror("shm_open"); return -1; }
  if (ftruncate(shm_fd, Messages_MaxSize) == -1) { perror("ftruncate"); return -1; }
  if ((shm_ptr = mmap(NULL,
                      Messages_MaxSize,
                      PROT_WRITE | PROT_READ,
                      MAP_SHARED,
                      shm_fd,
                      0)) == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(shm_fd) == -1) { perror("close"); return -1; }
  sprintf(shm_ptr, "<message>Hello world! From the producer process, with PID: %d.</message>", getpid());
  if (msync(shm_ptr, Messages_MaxSize, MS_SYNC | MS_INVALIDATE) == -1) { perror("msync"); return -1; }
  fprintf(stderr, "[Producer, PID: %d] I have written the following text in the shared memory region:\n%s\n", getpid(), shm_ptr);
  if (munmap(shm_ptr, Messages_MaxSize) == -1) { perror("munmap"); return -1; }
  return 0;
}