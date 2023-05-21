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

//  define error messages
#define rm_io_err { fprintf(stderr, "%s: Recieved an invalid option (%s).\n", __func__, option); return -1; }

#define cf_bw_err { fprintf(stderr, "%s: Fatal write error, the number of bytes that ahve been read (%ld) and the number of bytes that have been written (%ld) are not equal.\n", __func__, bytes_in, bytes_out); return -1; }
#define cf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -2; }
#define cf_w_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -3; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be %d.\n", __func__, argc - 1, 2); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -2; }
#define m_cr_err { fprintf(stderr, "%s: Error encountered while creating a file.\n", __func__); return -3; }
#define m_s_err { fprintf(stderr, "%s: Error encountered while recieving information about a directory (%s).\n", __func__, output_file_name); return -2; }
#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -4; }
#define m_rof_err { fprintf(stderr, "%s: Error encountered while recieving the output file.\n", __func__); return -5; }
#define m_rm_err { fprintf(stderr, "%s: Error encountered while recieving an option.\n", __func__); return -4; }
#define m_fc_err { fprintf(stderr, "%s: Can't copy a file into a newer file that already exists.\n", __func__); return -5; }
#define m_cf_err { fprintf(stderr, "%s: Error encountered while processing files.\n", __func__); return -6; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -7; }

char input_file_name[PATH_MAX];
char output_file_name[PATH_MAX];
int32_t input_fd;
int32_t output_fd;

unsigned char mode;
#define M_UPDATE 1
#define M_INTERACTIVE 2

//  -1 - invalid option
int32_t recieve_mode(unsigned char *m, char* option) {
  if (strcmp(option, "-u") == 0) {
    *m = *m | M_UPDATE;
  }
  else if (strcmp(option, "-i") == 0) {
    *m = *m | M_INTERACTIVE;
  }
  else rm_io_err
  return 0;
}


//basically copies the input file into the output file
//  -1 -bad write (impossible case)
//  -2 -read
//  -3 -write
int32_t copy_file(int32_t input_descriptor, int32_t output_descriptor) {
  ssize_t bytes_in;
  ssize_t bytes_out;
  char buffer[BUF_SIZE];

  while ((bytes_in  = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    bytes_out = write(output_descriptor, buffer, bytes_in);
    if (bytes_out != bytes_in) cf_bw_err
    if (bytes_out == -1) cf_w_err
  }
  if (bytes_in == -1) cf_r_err
  return 0;
}

//  -1 - bad argument count
//  -2 - open
//  -3 - creat
//  -4 - stat
//  -5 - recieve_input_file
//  -6 - recieve_output_file
//  -7 - recieve_mode
//  -8 - failed copy
//  -9 - copy_file
//  -10 - close
int32_t main(int32_t argc, char** argv) {
//  print_args(argc, argv);

  if (recieve_input_file(argc, argv, input_file_name, 1) < 0) m_rif_err

  int32_t i = 3;
  while (i < argc) {
    if (recieve_mode(&mode, argv[i]) < 0) m_rm_err
    ++i;
  }

  if ((input_fd = open(input_file_name, O_RDONLY, 0400)) == -1) m_o_err

  int32_t x = recieve_output_file(argc, argv, output_file_name, 2);
  if (x == 0) {
    if ((output_fd = creat(output_file_name, 0644)) == -1) m_cr_err
  }
  else if (x == 1) {
    struct stat is, os;
    if (stat(input_file_name, &is) == -1) m_s_err
    if (stat(output_file_name, &os) == -1) m_s_err
    if ((mode & M_UPDATE) && is.st_mtime < os.st_mtime) m_fc_err
    else {
      if (mode & M_INTERACTIVE) {
        char answer = 'x';
        while (!strchr("yn", answer)) {
          printf("The destination already exists, do you want to overwrite it? (y/n)\n");
          scanf("%c", &answer);
        }
        if (answer == 'n') {
          return 1;
        }
      }
      if ((output_fd = open(output_file_name, O_WRONLY | O_TRUNC, 0644)) == -1) m_o_err
    }
  }
  else if (x == -1) m_rof_err
  if (output_fd == -1) m_o_err

  if (copy_file(input_fd, output_fd) < 0) m_cf_err

  if (close(input_fd) == -1) m_cl_err

  if (close(output_fd) == -1) m_cl_err

  return 0;
}
#undef M_UPDATE
#undef M_INTERACTIVE

#undef rm_io_err

#undef cf_bw_err
#undef cf_r_err
#undef cf_w_err

#undef m_bac_err
#undef m_o_err
#undef m_cr_err
#undef m_s_err
#undef m_rif_err
#undef m_rof_err
#undef m_rm_err
#undef m_fc_err
#undef m_cf_err
#undef m_cl_err
