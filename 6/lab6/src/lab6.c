#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include <string.h>
#include <linux/limits.h>
#include <libgen.h>

#include "print_args.h"
#include "file_work/recieve_input_file.h"
#include "file_work/recieve_output_file.h"
#define BUF_SIZE 4096

#define dos2unix 0

//  define error messages
#define pf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -1; }
#define pf_w_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -2; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be %d.\n", __func__, argc - 1, 2); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -2; }
#define m_cr_err { fprintf(stderr, "%s: Error encountered while creating a file.\n", __func__); return -3; }
#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -4; }
#define m_rof_err { fprintf(stderr, "%s: Error encountered while recieving the output file.\n", __func__); return -5; }
#define m_pf_err { fprintf(stderr, "%s: Error encountered while processing files.\n", __func__); return -7; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -9; }

char input_file_name[PATH_MAX];
char output_file_name[PATH_MAX];
int32_t input_fd;
int32_t output_fd;

//  -1 -read
//  -2 -write
int32_t process_file(int32_t input_descriptor, int32_t output_descriptor) {
  ssize_t bytes_in;
  char buffer_in[BUF_SIZE];
  char buffer_out[BUF_SIZE];

  int64_t i = 0;     //buffer_in cursor
  int64_t o = 0;     //buffer_out cursor
  char last = 0;

  while ((bytes_in  = read(input_descriptor, buffer_in, BUF_SIZE)) > 0) {
    i = 0;
    while (i < bytes_in) {
#if dos2unix == 1         //13 10 => 10
      if (last == 13 && buffer_in[i] == 10) {
        buffer_out[o] = 10;
      }
#else                     //10 => 13 10
      if (buffer_in[i] == 10) {
        buffer_out[o] = 13;
        ++o;
        if (o == BUF_SIZE) {
          if (write(output_descriptor, buffer_out, BUF_SIZE) == -1) pf_w_err
          o = 0;
        }
        buffer_out[o] = 10;
      }
#endif
      else {
        buffer_out[o] = buffer_in[i];
      }
      last = buffer_in[i];
      ++i;
      ++o;
      if (o == BUF_SIZE) {
        if (write(output_descriptor, buffer_out, BUF_SIZE) == -1) pf_w_err
        o = 0;
      }
    }
  }
  if (o != 0) {
    if (write(output_descriptor, buffer_out, o) == -1) pf_w_err
  }
  if (bytes_in == -1) pf_r_err
  return 0;
}

//  -1 - bad argument count
//  -2 - open
//  -3 - creat
//  -4 - recieve_input_file
//  -5 - recieve_output_file
//  -6 - process_file
//  -7 - close
int32_t main(int32_t argc, char** argv) {
//  print_args(argc, argv);
  if (argc - 1 != 2) m_bac_err

  if (recieve_input_file(argc, argv, input_file_name, 1) < 0) m_rif_err

  if ((input_fd = open(input_file_name, O_RDONLY, 0400)) == -1) m_o_err

  int32_t x = recieve_output_file(argc, argv, output_file_name, 2);
  if (x == 0) {
    if ((output_fd = creat(output_file_name, 0644)) == -1) m_cr_err
  }
  else if (x == 1) {
    if ((output_fd = open(output_file_name, O_WRONLY | O_TRUNC, 0644)) == -1) m_o_err
  }
  else if (x == -1) m_rof_err
  if (output_fd == -1) m_o_err

  if (process_file(input_fd, output_fd) < 0) m_pf_err

  if(close(input_fd) == -1) m_cl_err

  if(close(output_fd) == -1) m_cl_err

  return 0;
}

#undef pf_r_err
#undef pf_w_err

#undef m_bac_err
#undef m_o_err
#undef m_cr_err
#undef m_rif_err
#undef m_rof_err
#undef m_pf_err
#undef m_cl_err
