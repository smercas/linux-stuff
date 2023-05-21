#define _GNU_SOURCE
#include <stdio.h>
#include <stdio_ext.h>
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
#define BUF_SIZE 4

//  define error messages
#define rm_io_err { fprintf(stderr, "%s: Recieved an invalid option (%s).\n", __func__, option); return -1; }

#define pf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -1; }

#define wf_apf_err { fprintf(stderr, "%s: Error encountered while converting a long int(%ld) into a string.\n", __func__, output_values[count]); return -1; }
#define wf_w_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -2; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be at least %d.\n", __func__, argc - 1, 2); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -2; }
#define m_cr_err { fprintf(stderr, "%s: Error encountered while creating a file.\n", __func__); return -3; }
#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -4; }
#define m_rof_err { fprintf(stderr, "%s: Error encountered while recieving the output file.\n", __func__); return -5; }
#define m_rm_err { fprintf(stderr, "%s: Error encountered while recieving an option.\n", __func__); return -4; }
#define m_rf_err { fprintf(stderr, "%s: Error encountered while reading the input file.\n", __func__); return -6; }
#define m_wf_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -5; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -6; }

char input_file_name[PATH_MAX];
char output_file_name[PATH_MAX];
int32_t input_fd;
int32_t output_fd;
uint64_t lines, words, bytes, length;
uint64_t details[4];

unsigned char mode;

#define M_LINES 1
#define M_WORDS 2
#define M_BYTES 4
#define M_LENGTH 8

//  -1 - invalid option
int32_t recieve_mode(unsigned char *m, char* option) {
  if (strcmp(option, "-l") == 0) {
    *m = *m | M_LINES;
  }
  else if (strcmp(option, "-w") == 0) {
    *m = *m | M_WORDS;
  }
  else if (strcmp(option, "-c") == 0) {
    *m = *m | M_BYTES;
  }
  else if (strcmp(option, "-L") == 0) {
    *m = *m | M_LENGTH;
  }
  else rm_io_err
  return 0;
}

//  -1 -read
int32_t read_file(int32_t input_descriptor, unsigned char mode, uint64_t *lines, uint64_t *words, uint64_t *bytes, uint64_t *length) {
  ssize_t bytes_in;
  char buffer[BUF_SIZE];

  char* empty = " \t\n\v\f\r";

  int64_t i = 0;

  char last = 0;
  //  0 - last char was a non-white space
  //  1 - last char was a white space
  uint64_t current_max = 0;

  *bytes = 0;
  *lines = 0;
  *length = 0;
  *words = 0;

  while ((bytes_in  = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    i = 0;
    while (i < bytes_in) {
      if (mode & M_BYTES) {
        ++(*bytes);
      }
      if (mode & M_LINES && buffer[i] == '\n') {
        ++(*lines);
      }
      if (mode & M_LENGTH) {
        if (buffer[i] == '\n') {
          if (current_max > *length) {
            *length = current_max;
          }
          current_max = 0;
        }
        else {
          ++current_max;
        }
      }
      if (mode & M_WORDS) {
        if (strchr(empty, buffer[i])) {
          if (last) {
            ++(*words);
          }
          last = 0;
        }
        else {
          last = 1;
        }
      }
      ++i;
    }
  }
  if (bytes_in == -1) pf_r_err
  if (last) {
    ++(*words);
  }
  return 0;
}

//  -1 - asprintf
//  -2 - write
int32_t write_file(int32_t output_descriptor, uint64_t* output_values, uint64_t output_count) {
  char buffer[BUF_SIZE];    //buffer
  char* minibuffer;         //buffer for the object that'll have to be written
  
  uint64_t count = 0;       //number of objects that have been written
  uint64_t i = 0;           //buffer position
  uint64_t j;               //minibuffer position

  while (count < output_count) {
    if (asprintf(&minibuffer, "%lu", output_values[count]) == -1) wf_apf_err
    j = 0;
    while (i + j < BUF_SIZE && j < strlen(minibuffer)) {
      buffer[i + j] = minibuffer[j];
      ++j;
    }
    if (i + j < BUF_SIZE && j == strlen(minibuffer)) {
      buffer[i + j] = (count == output_count - 1 ? '\n' : ' ');
      ++j;
    }
    i = i + j;

    while (i == BUF_SIZE) {
      if (write(output_descriptor, buffer, BUF_SIZE) == -1) wf_w_err
      i = 0;
      while (i < BUF_SIZE && j < strlen(minibuffer)) {
        buffer[i] = minibuffer[j];
        ++i;
        ++j;
      }
      if (i < BUF_SIZE && j == strlen(minibuffer)) {
        buffer[i] = (count == output_count - 1 ? '\n' : ' ');;
        ++i;
        ++j;
      }
      //same stuff as above but starting from 0 in the buffer and from j in minibuffer
      //this way objects that're either separated in different buffers or are
      //bigger than a normal buffer can be written using write
    }
    ++count;
    free(minibuffer);
  }
  if (i != 0) {
    if (write(output_descriptor, buffer, i) == -1) wf_w_err
  }
  return 0;
}

//  -1 - bad argument count
//  -2 - open
//  -3 - creat
//  -4 - recieve_input_file
//  -5 - recieve_output_file
//  -6 - recieve_mode
//  -7 - read_file
//  -8 - write_file
//  -9 - close
int32_t main(int32_t argc, char** argv) {
//  print_args(argc, argv);
  if (argc - 1 < 2) m_bac_err

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

  int32_t i = 3;
  while (i < argc) {
    if (recieve_mode(&mode, argv[i]) < 0) m_rm_err
    ++i;
  }

  if (mode == 0) {
    mode = mode | M_BYTES | M_LINES | M_WORDS;
  }

  if (read_file(input_fd, mode, &lines, &words, &bytes, &length) < 0) m_rf_err

  i = 0;

  if (mode & M_LINES) {
    details[i] = lines;
    ++i;
  }
  if (mode & M_WORDS) {
    details[i] = words;
    ++i;
  }
  if (mode & M_BYTES) {
    details[i] = bytes;
    ++i;
  }
  if (mode & M_LENGTH) {
    details[i] = length;
    ++i;
  }

  x = 0;
  while (x < i) {
    printf("%lu ", details[x]);
    ++x;
  }
  printf("\n");

  if (write_file(output_fd, details, i) < 0) m_wf_err

  if(close(input_fd) == -1) m_cl_err

  if(close(output_fd) == -1) m_cl_err

  return 0;
}

#undef M_LINES
#undef M_WORDS
#undef M_BYTES
#undef M_LENGTH

#undef rm_io_err

#undef pf_r_err

#undef wf_apf_err
#undef wf_w_err

#undef m_bac_err
#undef m_o_err
#undef m_cr_err
#undef m_rif_err
#undef m_rof_err
#undef m_rm_err
#undef m_rf_err
#undef m_wf_err
#undef m_cl_err
