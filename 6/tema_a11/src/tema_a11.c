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
#include "file_work/recieve_io_file.h"
#define BUF_SIZE 4096

//  define error messages
#define pf_bw_err { fprintf(stderr, "%s: Fatal write error, the number of bytes that have been read (%ld) and the number of bytes that have been written (%ld) are not equal.\n", __func__, bytes_in, bytes_out); return -1; }
#define pf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -2; }
#define pf_ls_err { fprintf(stderr, "%s: Error encountered while readjusting the file cursor.\n", __func__); return -5; }
#define pf_w_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -4; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be either %d or %d.\n", __func__, argc - 1, 1, 2); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -2; }
#define m_cr_err { fprintf(stderr, "%s: Error encountered while creating a file.\n", __func__); return -3; }
#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -4; }
#define m_rof_err { fprintf(stderr, "%s: Error encountered while recieving the output file.\n", __func__); return -5; }
#define m_riof_err { fprintf(stderr, "%s: Error encountered while recieving the input-output file.\n", __func__); return -6; }
#define m_pf_err { fprintf(stderr, "%s: Error encountered while processing files.\n", __func__); return -7; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -8; }

char input_file_name[PATH_MAX];
char output_file_name[PATH_MAX];
int32_t input_fd;
int32_t output_fd;
int32_t io_fd;
char mode;

//basically copies the input file into the output file
//  -1 - bad write (impossible case)
//  -2 - read
//  -3 - lseek
//  -4 - write
int32_t process_file(int32_t input_descriptor, int32_t output_descriptor) {
  ssize_t bytes_in;
  ssize_t bytes_out;
  char buffer[BUF_SIZE];
  int64_t i;

  while ((bytes_in  = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    i = 0;
    while (i < bytes_in) {
      if (buffer[i] >= 'a' && buffer[i] <= 'z') {
        buffer[i] = buffer[i] - 32;
      }
      ++i;
    }
    if (input_descriptor == output_descriptor) {
      if (lseek(output_descriptor, -bytes_in, SEEK_CUR) == -1) pf_ls_err
    }
    bytes_out = write(output_descriptor, buffer, bytes_in);
    if (bytes_out != bytes_in) pf_bw_err
    if (bytes_out == -1) pf_w_err
  }
  if (bytes_in == -1) pf_r_err
  return 0;
}

//  -1 - bad argument count
//  -2 - open
//  -3 - creat
//  -4 - recieve_input_file
//  -5 - recieve_output_file
//  -6 - recieve_io_file
//  -7 - process_file
//  -8 - close
int32_t main(int32_t argc, char** argv) {
//  print_args(argc, argv);
  int32_t x;

  if (argc - 1 != 1 && argc - 1 != 2) m_bac_err
  else if (argc - 1 == 1) {
    mode = 1;
    x = recieve_io_file(argc, argv, input_file_name, 1);
    if (x == 0) {
      if ((io_fd = creat(input_file_name, 0600)) == -1) m_cr_err
      if (close(io_fd) == -1) m_cl_err                                  //if you don't close the
      if ((io_fd = open(input_file_name, O_RDWR, 0600)) == -1) m_o_err //file you can't read from it
    }
    else if (x == 1) {
      if ((io_fd = open(input_file_name, O_RDWR, 0600)) == -1) m_o_err
    }
    else if (x == -1) m_riof_err
  }
  else {
    if (recieve_input_file(argc, argv, input_file_name, 1) < 0) m_rif_err
    x = recieve_output_file(argc, argv, output_file_name, 2);
    if (x == 0) {
      mode = 0;
      if ((input_fd = open(input_file_name, O_RDONLY, 0400)) == -1) m_o_err
      if ((output_fd = creat(output_file_name, 0600)) == -1) m_cr_err
    }
    else if (x == 1) {
      if (strcmp(input_file_name, output_file_name) == 0) {
        mode = 1;
        if ((io_fd = open(input_file_name, O_RDWR, 0600)) == -1) m_o_err
      }
      else {
        char answer = 'x';
        while (!strchr("yn", answer)) {
          printf("The output file already exists, do you want to overwrite it? (y/n)\n");
          scanf("%c", &answer);
        }
        if (answer == 'n') {
          return 1;
        }
        else {
          mode = 0;
          
          if ((input_fd = open(input_file_name, O_RDONLY, 0400)) == -1) m_o_err
          if ((output_fd = open(output_file_name, O_WRONLY | O_TRUNC, 0600)) == -1) m_o_err
        }
      }
    }
    else if (x == -1) m_rof_err
  }

  if (mode == 0) {
    if (process_file(input_fd, output_fd) < 0) m_pf_err
    if (close(input_fd) == -1) m_cl_err
    if (close(output_fd) == -1) m_cl_err
  }
  else if (mode == 1) {
    if (process_file(io_fd, io_fd) < 0) m_pf_err
    if (close(io_fd) == -1) m_cl_err
  }

  return 0;
}

#undef pf_bw_err
#undef pf_r_err
#undef pf_ls_err
#undef pf_w_err

#undef m_bac_err
#undef m_o_err
#undef m_c_err
#undef m_rif_err
#undef m_rof_err
#undef m_riof_err
#undef m_pf_err
#undef m_c_err
