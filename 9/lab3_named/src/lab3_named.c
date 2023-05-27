#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct shm_struct {
  uint64_t shm_cnt_data;
  uint64_t shm_cnt_instances;
};

struct shm_struct* shm_ptr;

void critical_section() {
  ++shm_ptr->shm_cnt_data;	
}

void remainder_section() {
}


int32_t main(int32_t argc, char**argv) {
  uint64_t nr_iterations, i, nlocal_exit_order;
  const char* shm_name = "/shared_memory_obj1";
  const char* sem_name = "/named_semaphore_obj1";
  sem_t* my_semaphore;
  int32_t shm_fd;
  if (argc - 1 != 1) { fprintf(stderr,"Usage: number_of_iterations\n"); return -1; }
  if (sscanf(argv[1], "%lu", &nr_iterations) != 1) { fprintf(stderr, "Invalid number of iterations: %s\n", argv[1]); return -1; }
  if ((my_semaphore = sem_open(sem_name, O_RDWR | O_CREAT, 0600, 1)) == SEM_FAILED) { perror("sem_open"); return -1; }
  if ((shm_fd = shm_open(shm_name, O_RDWR | O_CREAT, 0600)) == -1) { perror("shm_open"); return -1; }
  if (ftruncate(shm_fd, sizeof(*shm_ptr)) == -1) { perror("ftruncate"); return -1; }
  if ((shm_ptr = mmap(NULL,
                      sizeof(*shm_ptr),
                      PROT_READ | PROT_WRITE,
                      MAP_SHARED,
                      shm_fd,
                      0)) == MAP_FAILED) { perror("mmap"); return -1; }
  if (close(shm_fd) == -1) { perror("close"); return -1; }
  if (sem_wait(my_semaphore) == -1) { perror("beginning sem_wait"); return -1; }
  //fprintf(stderr, "%lu -> %lu\n", shm_ptr->shm_cnt_instances, shm_ptr->shm_cnt_instances + 1);
  ++shm_ptr->shm_cnt_instances;
  if (sem_post(my_semaphore) == -1) { perror("beginning sem_post"); return -1; }
  usleep(1000); // not necessary if you do some more complex stuff inside the program
  i = 1;
  while (i <= nr_iterations) {
    if (sem_wait(my_semaphore) == -1) { perror("sem_wait"); return -1; }
    critical_section();
    if (sem_post(my_semaphore) == -1) { perror("sem_post"); return -1; }
    remainder_section();
    ++i;
  }
  if (sem_wait(my_semaphore) == -1) { perror("ending sem_wait"); return -1; }
  //fprintf(stderr, "%lu -> %lu\n", shm_ptr->shm_cnt_instances, shm_ptr->shm_cnt_instances - 1);
  nlocal_exit_order = shm_ptr->shm_cnt_instances;
  --shm_ptr->shm_cnt_instances;
  if (sem_post(my_semaphore) == -1) { perror("ending sem_post"); return -1; }
  if (sem_close(my_semaphore) == -1) { perror("sem_close"); return -1; }
  fprintf(stderr, "[PID: %d]\tI finished my last iteration in CS on the %luth place (in reverse order).\n", getpid(), nlocal_exit_order);
  if (nlocal_exit_order == 1) {
    if (sem_unlink(sem_name) == -1) { perror("sem_unlink"); return -1; }
    if (shm_unlink(shm_name) == -1) { perror("shm_unlink"); return -1; }
    printf("[PID: %d] The final value of the shared counter is: %lu\n", getpid(), shm_ptr->shm_cnt_data);
  }
  if (munmap(shm_ptr, sizeof(*shm_ptr)) == -1) { perror("munmap"); return -1; }
  return 0;
}
