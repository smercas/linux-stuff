#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define m_bac_err { fprintf(stderr, "%s: The number of arguments that have been passed is %d when it should be %d.\n", __func__, argc - 1, 3); return -1; }
#define m_sc_err { fprintf(stderr, "%s: Error encountered while recieving details about the system configuration.\n", __func__); return -2; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -3; }
#define m_s_err { fprintf(stderr, "%s: Error encountered while recieving information about a file.\n", __func__); return -4; }
#define m_opeof_err { fprintf(stderr, "%s: File offset(%ld) is past the end of the file(%ld).\n", __func__, offset, s.st_size); return -5; }
#define m_mf_err {fprintf(stderr, "%s: Failed to create a map.\n", __func__); return -6; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -7; }
#define m_w_err { fprintf(stderr, "%s: Error encountered while writing in the console.\n", __func__); return -8; }
#define m_pw_err { fprintf(stderr, "%s: Error encountered while writing in the console (partial write completed).\n", __func__); return -9; }
#define m_um_err { fprintf(stderr, "%s: Failed to unmap a memory region.\n", __func__); return -10; }

//   -1 - bad argument count
//   -2 - sysconf
//   -3 - open
//   -4 - stat
//   -5 - offset set past the end of the file
//   -6 - map failed
//   -7 - close
//   -8 - write
//   -9 - partial write
//  -10 - munmap
int32_t main(int32_t argc, char** argv) {
  char* map_address;
  int32_t fd;
  struct stat s;
  off_t offset, page_alligned_offset;
  uint64_t length, adjusted_length;
  int64_t write_code;
  int64_t page_size;
  
  if (argc - 1 != 3) m_bac_err
  if ((page_size = sysconf(_SC_PAGE_SIZE)) == -1) m_sc_err
  if ((fd = open(argv[1], O_RDONLY)) == -1) m_o_err;
  if (fstat(fd, &s) == -1) m_s_err
  offset = atoll(argv[2]);
  if (offset >= s.st_size) m_opeof_err
  page_alligned_offset = offset / page_size * page_size;
  length = strtoull(argv[3], NULL, 10);
  if (offset + (off_t)(length) > s.st_size) {
    length = s.st_size - offset;
  }
  adjusted_length = length + offset - page_alligned_offset;
  map_address = mmap(NULL,
                     adjusted_length,
                     PROT_READ,
                     MAP_PRIVATE,
                     fd,
                     page_alligned_offset);
  if (map_address == MAP_FAILED) m_mf_err
  if (close(fd) == -1) m_cl_err
  write_code = write(STDOUT_FILENO, map_address + offset - page_alligned_offset, length);
  if (write_code == -1) m_w_err
  else if (write_code != (int64_t)length) m_pw_err
  if (munmap(map_address, adjusted_length) == -1) m_um_err
  return 0;
}
#undef m_bac_err
#undef m_sc_err
#undef m_o_err
#undef m_s_err
#undef m_opeof_err
#undef m_mf_err
#undef m_cl_err
#undef m_w_err
#undef m_pw_err
