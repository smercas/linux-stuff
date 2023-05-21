#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdio.h>
#include <stdlib.h>

#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -1; }
#define m_ft_err { fprintf(stderr, "%s: Failed to resize a file.\n", __func__); return -2; }
#define m_mf_err { fprintf(stderr, "%s: Failed to create a map.\n", __func__); return -3; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -4; }
#define m_ul_err { fprintf(stderr, "%s: Failed to unlink a space of shared memory object.\n", __func__); return -5; }

//  -1 - shm_open
//  -2 - ftruncate
//  -3 - mmap
//  -4 - close
//  -5 - shm_unlink
int main()
{
  // const int Page_Size = 4096;
  const int Messages_MaxSize = 100;
  const char *shm_name = "/shared_memory_obj1"; // The name of the POSIX shared memory object which will be created.
  char* shm_ptr;
  int shm_fd;

  if ((shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0600)) == -1) m_o_err

  if (ftruncate(shm_fd, Messages_MaxSize) == -1) m_ft_err

  if ((shm_ptr = mmap(NULL,
                      Messages_MaxSize,
                      PROT_WRITE | PROT_READ,
                      MAP_SHARED,
                      shm_fd,
                      0)) == MAP_FAILED) m_mf_err

  if (close(shm_fd) == -1) m_cl_err

  fprintf(stderr,"[Producer, PID: %d] I have written the following text in the shared memory region:\n%s\n",
      getpid(), (char *)shm_ptr );

  /* Eventually, we will destroy the object, but only after the consumer gets to read it. */
  //if(shm_unlink(shm_name) == -1) m_ul_err
  return 0;
}
#undef m_o_err
#undef m_ft_err
#undef m_mf_err
#undef m_cl_err
#undef m_ul_err
