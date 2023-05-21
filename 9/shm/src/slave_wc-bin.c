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
    const char *shm_name = "/shared_memory_obj1"; // Same name as in the producer program.
    char *start_addr;
    int shm_fd;

  if ((shm_fd = shm_open(shm_name, O_RDONLY, 0600)) == -1) m_o_err


    if((start_addr = mmap(NULL,
                          Messages_MaxSize,
                          PROT_READ,
                          MAP_SHARED,
                          shm_fd,
                          0)) == MAP_FAILED)
    if(close(shm_fd) == -1) m_cl_err

    /* Read a message from the shared memory object. */
    if(! (start_addr[0] && start_addr[1] && start_addr[2]) )
        printf("[Consumer, PID: %d] In the shared memory there are only zeros (at least at the first 3 addresses) !\n",
            getpid()); // This can happen only if the consumer reads the memory before the producer writes it!
    else
        printf("[Consumer, PID: %d] I have read the following text from the shared memory region:\n%s\n",
            getpid(), start_addr);

    /* Eventually, we can destroy the object now, but only if it was not destroyed in the producer,
       because it will give an error if it is destroyed 2 times ! */

    // if(-1 == shm_unlink(shm_name)) handle_error("Error at shm_unlink",5);

    return 0;
}