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
  if ((shm_fd = shm_open(shm_name, O_RDONLY, 0)) == -1) { perror("shm_open"); return -1; }
  if ((shm_ptr = mmap(NULL,
                      Messages_MaxSize,
                      PROT_READ,
                      MAP_SHARED,
                      shm_fd,
                      0)) == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(shm_fd) == -1) { perror("close"); return -1; }
  if (shm_ptr[0] == '\0') {
    printf("[Consumer, PID: %d] No message to be read!\n", getpid());
  }
  else {
    printf("[Consumer, PID: %d] I have read the following text from the shared memory region:\n%s\n",
          getpid(), shm_ptr);
  }
  if (munmap(shm_ptr, Messages_MaxSize) == -1) { perror("munmap"); return -1; }
  // if (shm_unlink(shm_name) == -1) { perror("shm_unlink"); return -1; }
  return 0;
}