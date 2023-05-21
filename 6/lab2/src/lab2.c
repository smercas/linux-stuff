#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <linux/limits.h>
#include <libgen.h>
#include "print_args.h"
#include "file_work/is_regular_file.h"
#include "file_work/recieve_input_file.h"
#include "file_work/recieve_input_file.h"
#define BUF_SIZE 4096

#define pf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -1; }

#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening the input file.\n", __func__); return -2; }
#define m_pf_err { fprintf(stderr, "%s: Error encountered while processing the input file.\n", __func__); return -3; }
#define m_c_err { fprintf(stderr, "%s: Error encountered while closing the input file.\n", __func__); return -4; }

char input_file_name[PATH_MAX];
int32_t input_fd;

uint32_t result[256];

//  -1 - read
int32_t process_file(int32_t input_descriptor, uint32_t *result) {
  ssize_t bytes_in;
  unsigned char buffer[BUF_SIZE];

  ;
  while ((bytes_in = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    for (uint32_t i = 0; i < bytes_in; ++i) {
      ++result[buffer[i]];
    }
  }

  if (bytes_in == -1) pf_r_err
  return 0;
}

void print_result(uint32_t* result) {
  for (uint32_t i = 0; i < 256; ++i) {
    if (result[i] > 0) {
      printf("The character %c appears %u times\n", i, result[i]);
    }
  }
}

//  -1 - recieve_input_file
//  -2 - open
//  -3 - process_file
//  -4 - close
int32_t main(int32_t argc, char** argv) {
  print_args(argc, argv);

  if (recieve_input_file(argc, argv, input_file_name, 1) < 0) m_rif_err

  if ((input_fd = open(input_file_name, O_RDONLY, 0400)) == -1) m_o_err

  if (process_file(input_fd, result) < 0) m_pf_err

  print_result(result);

  if(close(input_fd) == -1) m_c_err

  return 0;
}

#undef pf_r_err

#undef m_rif_err
#undef m_o_err
#undef m_pf_err
#undef m_c_err