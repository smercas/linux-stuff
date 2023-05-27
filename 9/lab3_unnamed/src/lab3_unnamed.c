#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

struct shm_struct {
  uint64_t shm_cnt_data;
  uint64_t shm_cnt_instances;
  sem_t shm_semaphore;
  bool shm_semaphore_was_initialized;
};

struct shm_struct *shm_ptr;

void critical_section() {
  ++shm_ptr->shm_cnt_data;
}

void remainder_section() {
}

int32_t main(int32_t argc, char** argv) {
  uint64_t nr_iterations, nlocal_exit_order, i;
  const char *shm_name = "/shared_memory_obj1";
  int32_t shm_fd;
  bool first_at_start = true;
  if (argc - 1 != 1) { fprintf(stderr, "Usage: number_of_iterations\n"); return -1; }
  if (sscanf(argv[1],"%lu", &nr_iterations) != 1) { fprintf(stderr,"Invalid argument: %s\n", argv[1]); return -1; }
  if ((shm_fd = shm_open(shm_name, O_RDWR | O_CREAT | O_EXCL, 0600)) == -1) {
    if (errno == EEXIST) {
      first_at_start = false;
      if ((shm_fd = shm_open(shm_name, O_RDWR, 0600)) == -1) { perror("subsequent shm_open"); return -1; }
    }
    else { perror("first shm_open"); return -1; }
  }
  if (ftruncate(shm_fd, sizeof(*shm_ptr)) == -1) { perror("ftruncate"); return -1; }
  if ((shm_ptr = mmap(NULL,
                      sizeof(*shm_ptr),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      shm_fd,
                      0)) == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(shm_fd) == -1) { perror("close"); return -1; }
  if (first_at_start) {
    if (sem_init(&shm_ptr->shm_semaphore, 1, 1) == -1) { perror("sem_init"); return -1; }
    shm_ptr->shm_semaphore_was_initialized = true;
  }
  else { do { usleep(1000); } while (!shm_ptr->shm_semaphore_was_initialized); }
  if (sem_wait(&shm_ptr->shm_semaphore) == -1) { perror("beginning sem_wait"); return -1; }
  //fprintf(stderr, "%lu -> %lu\n", shm_ptr->shm_cnt_instances, shm_ptr->shm_cnt_instances + 1);
  ++shm_ptr->shm_cnt_instances;
  if (sem_post(&shm_ptr->shm_semaphore) == -1) { perror("beginning sem_post"); return -1; }
  usleep(10000); // not necessary if you do some more complex stuff inside the program
  i = 1;
  while (i <= nr_iterations) {
    if (sem_wait(&shm_ptr->shm_semaphore) == -1) { perror("sem_wait"); return -1; }
    critical_section();
    if (sem_post(&shm_ptr->shm_semaphore) == -1) { perror("sem_post"); return -1; }
    remainder_section();
    ++i;
  }
  if (sem_wait(&shm_ptr->shm_semaphore) == -1) { perror("ending sem_wait"); return -1; }
  //fprintf(stderr, "%lu -> %lu\n", shm_ptr->shm_cnt_instances, shm_ptr->shm_cnt_instances - 1);
  nlocal_exit_order = shm_ptr->shm_cnt_instances;
  --shm_ptr->shm_cnt_instances;
  if (sem_post(&shm_ptr->shm_semaphore) == -1) { perror("ending sem_post"); return -1; }
  fprintf(stderr,"[PID: %d]\tI finished my last iteration in CS on the %luth place (in reverse order).\n", getpid(), nlocal_exit_order);
  if (nlocal_exit_order == 1) {
    if (sem_destroy(& shm_ptr->shm_semaphore) == -1) { perror("sem_destroy"); return -1; }
    if (shm_unlink(shm_name) == -1) { perror("shm_unlink"); return -1; }
    printf("[PID: %d] The final value of the shared counter is: %lu\n", getpid(), shm_ptr->shm_cnt_data);
  }
  if (munmap(shm_ptr, sizeof(*shm_ptr)) == -1) { perror("munmap"); return -1; }
  return 0;
}
