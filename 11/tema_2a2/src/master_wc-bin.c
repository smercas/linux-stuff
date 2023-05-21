#define _GNU_SOURCE
#include <stdio_ext.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <fcntl.h>

#define m_bac_err { fprintf(stderr, "%s: The number of arguments that have been passed is %d when it should be %d.\n", __func__, argc - 1, 3); return -1; }
#define m_sc_err { fprintf(stderr, "%s: Error encountered while recieving details about the system configuration.\n", __func__); return -2; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -3; }
#define m_s_err { fprintf(stderr, "%s: Error encountered while recieving information about a file.\n", __func__); return -4; }
#define m_opeof_err { fprintf(stderr, "%s: File offset(%ld) is past the end of the file(%ld).\n", __func__, offset, s.st_size); return -5; }
#define m_mf_err { fprintf(stderr, "%s: Failed to create a map.\n", __func__); return -6; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -7; }
#define m_pw_err { fprintf(stderr, "%s: Error encountered while writing in the console (partial write completed).\n", __func__); return -8; }
#define m_um_err { fprintf(stderr, "%s: Failed to unmap a memory region.\n", __func__); return -9; }
#define m_f_err { fprintf(stderr, "%s: Error encountered while creating a child process.\n", __func__); return -10; }
#define m_w_err { fprintf(stderr, "%s: Error encountered while waiting for a child process to end.\n", __func__); return -11; }

//   -1 - bad argument count
//   -2 - sysconf
//   -3 - open
//   -4 - stat
//   -5 - offset set past the end of the file
//   -6 - map failed
//   -7 - close
//   -8 - partial write
//   -9 - munmap
//  -10 - fork
//  -11 - wait
int32_t main(int32_t argc, char* argv[]) {
  int32_t code;
  uint64_t n, i;
  pid_t pid;
  int32_t* slave;
  char first_number[128];
  char second_number[128];
  char third_number[128];
  
  char* map_address;
  int32_t fd;
  struct stat s;
  int64_t length;
  int64_t write_code;
  int64_t page_size;

  if (argc - 1 != 2) m_bac_err

  if ((page_size = sysconf(_SC_PAGE_SIZE)) == -1) m_sc_err
  if ((fd = open(argv[0], O_RDONLY), 0600) == -1) m_o_err
  if (fstat(fd, &s) == -1) m_s_err
  length = s.st_size;
  map_address = mmap(NULL,
                     length,
                     PROT_READ,
                     MAP_PRIVATE,
                     fd,
                     0);
  if (map_address == MAP_FAILED) m_mf_err
  if (close(fd) == -1) m_cl_err

  n = strtoull(argv[2], NULL, 10);
  slave = (int32_t*)malloc(sizeof(int32_t) * n);

  for(i = 1; i < n; ++i) {
    switch (pid = fork()) {
      case -1:
        m_f_err
        break;
      case  0:
        sprintf(first_number, "%lu", i * (length / n));
        sprintf(second_number, "%lu", length / n);
        sprintf(third_number, "%lu", i * 12);
        execl("slave", "slave", first_number, second_number, NULL);
      default:
        slave[i] = pid;
        break;
    }
  }

  for(i = 1; i <= n; ++i) {
    if (waitpid(slave[i], NULL, 0) == -1) m_w_err
  }

  free(slave);
  if (munmap(map_address, length) == -1) m_um_err
  return 0;
}
#undef m_bac_err
#undef m_sc_err
#undef m_o_err
#undef m_s_err
#undef m_opeof_err
#undef m_mf_err
#undef m_cl_err
#undef m_pw_err
#undef m_f_err
#undef m_w_err
