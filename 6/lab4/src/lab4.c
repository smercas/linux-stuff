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
#include <regex.h>

#include "print_args.h"
#include "file_work/recieve_input_file.h"
#include "file_work/recieve_output_file.h"
#define BUF_SIZE 4096

//  define error messages
#define cfvn_re_err { fprintf(stderr, "%s: Error encountered while matching a char array to a reguar expression.\n", __func__); return -1; }
#define cfvn_nm_err { fprintf(stderr, "%s: File contains something that is not a number (%s).\n", __func__, mb); return -2; }

#define rf_m_err { fprintf(stderr, "%s: Failed to allocate memory on the heap.\n", __func__); return -1; }
#define rf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -2; }
#define rf_cfvn_err { fprintf(stderr, "%s: Error encountered while checking if a sequence of characters is a valid number.\n", __func__); return -3; }
#define rf_bc_err { fprintf(stderr, "%s: File contains a bad character (%c).\n", __func__, buffer[i]); return -4; }
#define rf_ls_err { fprintf(stderr, "%s: Error encountered while readjusting the file cursor.\n", __func__); return -5; }
#define rf_an_err { fprintf(stderr, "%s: Error encountered while adding a number into the numbers list.\n", __func__); return -6; }
#define rf_rc_err { fprintf(stderr, "%s: Error encountered while compiling a regular expression. (%s)\n", __func__, pattern); return -7; }

#define ca_m_err { fprintf(stderr, "%s: Failed to allocate memory on the heap.\n", __func__); return -1; }

#define wf_apf_err { fprintf(stderr, "%s: Error encountered while converting a long int(%ld) into a string.\n", __func__, output_values[count]); return -1; }
#define wf_w_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -2; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be %d.\n", __func__, argc - 1, 2); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -2; }
#define m_cr_err { fprintf(stderr, "%s: Error encountered while creating a file.\n", __func__); return -3; }
#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -4; }
#define m_rof_err { fprintf(stderr, "%s: Error encountered while recieving the output file.\n", __func__); return -5; }
#define m_rf_err { fprintf(stderr, "%s: Error encountered while reading the input file.\n", __func__); return -6; }
#define m_ca_err { fprintf(stderr, "%s: Error encountered while calculating the averages for the numbers list.\n", __func__); return -7; }
#define m_wf_err { fprintf(stderr, "%s: Error encountered while writing into the output file.\n", __func__); return -8; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -9; }

char input_file_name[PATH_MAX];
char output_file_name[PATH_MAX];
int32_t input_fd;
int32_t output_fd;

int64_t* numbers;
uint64_t number_count;

int64_t* averages;
uint64_t average_count;

//  -1 - regexec
//  -2 - no match
int32_t check_for_valid_number(char* mb, uint64_t *size, uint64_t *count, regex_t *reg) {
  if ((*size) > 0) {
    int32_t ret = regexec(&(*reg), mb, 0, NULL, 0);
    if (ret == 0) {
      //match found, keep going
    (*size) = 0;
    ++(*count);
    return 1;
    }
    else if (ret == REG_NOMATCH) cfvn_nm_err
    else cfvn_re_err
  }
  return 0;
}

int32_t add_number(char* mb, uint64_t *size, int64_t* *iv, uint64_t *ic) {
  if ((*size) > 0) {
    mb[*size] = '\0';
    (*iv)[*ic] = strtoll(mb, NULL, 10);
    (*size) = 0;
    ++(*ic);
    return 1;
  }
  return 0;
}

//  -1 - malloc
//  -2 - read
//  -3 - check_for_valid_number
//  -4 - bad char
//  -5 - lseek
//  -6 - add_number
//  -7 - regcomp
int32_t read_file(int32_t input_descriptor, int64_t* *input_values, uint64_t *input_count) {
  ssize_t bytes_in;
  char buffer[BUF_SIZE];

  uint64_t current_char_size = 0;
  uint64_t max_char_size = 8;

  char* minibuffer = (char*)malloc(sizeof(char) * max_char_size);
  char* aux;

  ssize_t i;

  //the first while is used in order to validate the elements of the file
  //and, more importanty, to count the number of elements so memory in the heap can be allocated only once


  regex_t regex;
  char* pattern = "^[-]?([1-9][0-9]*|0)$";
  int32_t regret = regcomp(&regex, pattern, REG_EXTENDED);
  if (regret != 0) rf_rc_err
  while ((bytes_in = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    i = 0;
    while (i < bytes_in) {
      if ((buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '-') {
        if (!(current_char_size < max_char_size - 1)) {   //this way there's always space for the '\0' character
          max_char_size = max_char_size * 2;
          aux = (char*)malloc(sizeof(char) * max_char_size);
          if (aux == NULL) rf_m_err
          for (uint64_t j = 0; j < current_char_size; ++j) {
            aux[j] = minibuffer[j];
          }
          free(minibuffer);
          minibuffer = aux;
          aux = NULL;
        }
        minibuffer[current_char_size] = buffer[i];            //adds the i-th buffer character to the minibuffer
        ++current_char_size;
      }
      else if (buffer[i] == ' ' || buffer[i] == '\n') {
        minibuffer[current_char_size] = '\0';                 //adds '\0' to the minibuffer
        
        if (check_for_valid_number(minibuffer, &current_char_size, &*input_count, &regex) < 0) rf_cfvn_err
      }
      else rf_bc_err
      ++i;
    }
  }
  if (bytes_in == -1) rf_r_err

  if (check_for_valid_number(minibuffer, &current_char_size, &*input_count, &regex) < 0) rf_cfvn_err

  regfree(&regex);

  //if the function hasn't ended yet, then all the objects that are in the input file are correct
  
  if (lseek(input_descriptor, 0, SEEK_SET) == -1) rf_ls_err
  *input_values = (int64_t*)malloc(sizeof(int64_t) * (*input_count));
  if (*input_values == NULL) rf_m_err

  (*input_count) = 0;

  //the second while is used to actually read and store the objects

  while ((bytes_in = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    i = 0;
    while (i < bytes_in) {
      if ((buffer[i] >= '0' && buffer[i] <= '9') || buffer[i] == '-') {
        minibuffer[current_char_size] = buffer[i];
        ++current_char_size;
      }
      else if (buffer[i] == ' ' || buffer[i] == '\n') {
        if (add_number(minibuffer, &current_char_size, &*input_values, &*input_count) < 0) rf_an_err
      }
      ++i;
    }
  }
  if (bytes_in == -1) rf_r_err
  if (add_number(minibuffer, &current_char_size, &*input_values, &*input_count) < 0) rf_an_err
  free (minibuffer);
  return 0;
}

//  -1 - malloc
int32_t calculate_averages(int64_t* v, uint64_t vc, int64_t* *a, uint64_t *ac) {
  *ac = 0;
  uint64_t i = 0;
  int64_t temp = 0;
  uint64_t temp_count;
  while (i < vc) {
    if (v[i] == 0) {
      ++(*ac);
    }
    ++i;
  }
  ++(*ac);
  *a = (int64_t*)malloc(sizeof(int64_t) * (*ac));
  if (*a == NULL) ca_m_err
  i = 0;
  *ac = 0;
  temp_count = 0;
  while (i < vc) {
    if (v[i] != 0) {
      temp = temp + v[i];
      ++temp_count;
    }
    else {
      if (temp_count == 0) {
        (*a)[*ac] = 0;
      }
      else {
        (*a)[*ac] = temp / temp_count;
      }
      ++(*ac);
      temp = 0;
      temp_count = 0;
    }
    ++i;
  }
  if (temp_count == 0) {
    (*a)[*ac] = 0;
  }
  else {
    (*a)[*ac] = temp / temp_count;
  }
  ++(*ac);
  return 0;
}

//  -1 - asprintf
//  -2 - write
int32_t write_file(int32_t output_descriptor, int64_t* output_values, uint64_t output_count) {
  char buffer[BUF_SIZE];    //buffer
  char* minibuffer;         //buffer for the object that'll have to be written
  
  uint64_t count = 0;       //number of objects that have been written
  uint64_t i = 0;           //buffer position
  uint64_t j;               //minibuffer position

  while (count < output_count) {
    if (asprintf(&minibuffer, "%ld", output_values[count]) == -1) wf_apf_err
    j = 0;
    while (i + j < BUF_SIZE && j < strlen(minibuffer)) {
      buffer[i + j] = minibuffer[j];
      ++j;
    }
    if (i + j < BUF_SIZE && j == strlen(minibuffer)) {    //usually this is '\0' but '\0' is not a good separator
      buffer[i + j] = '\n';                               //also you don't need '\0' when printing something
      ++j;
    }
    i = i + j;

    while (i == BUF_SIZE) {                               //will modify the buffer until it's no longer filled
      if (write(output_descriptor, buffer, BUF_SIZE) == -1) wf_w_err
      i = 0;
      while (i < BUF_SIZE && j < strlen(minibuffer)) {
        buffer[i] = minibuffer[j];
        ++i;
        ++j;
      }
      if (i < BUF_SIZE && j == strlen(minibuffer)) {
        buffer[i] = '\n';
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
//  -6 - read_file
//  -7 - calculate_averages
//  -8 - write_file
//  -9 - close
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

/*
  if (process_file(input_fd, output_fd) < 0) m_pf_err
*/

  if (read_file(input_fd, &numbers, &number_count) < 0) m_rf_err

/*
  uint64_t i = 0;
  while (i < number_count) {
    printf("%lu: %ld\n", i, numbers[i]);
    ++i;
  }
  printf("\n");
*/

  if (calculate_averages(numbers, number_count, &averages, &average_count) < 0) m_ca_err

/*
  i = 0;
  while (i < average_count) {
    printf("%lu: %ld\n", i, averages[i]);
    ++i;
  }
  printf("\n");
*/

  if (write_file(output_fd, averages, average_count) < 0) m_wf_err

  free(numbers);

  if(close(input_fd) == -1) m_cl_err

  if(close(output_fd) == -1) m_cl_err

  return 0;
}

#undef cfvn_re_err
#undef cfvn_nm_err

#undef rf_m_err
#undef rf_r_err
#undef rf_cfvn_err
#undef rf_bc_err
#undef rf_ls_err
#undef rf_an_err
#undef rf_rc_err

#undef ca_m_err

#undef wf_apf_err
#undef wf_w_err

#undef m_bac_err
#undef m_o_err
#undef m_cr_err
#undef m_rif_err
#undef m_rof_err
#undef m_rf_err
#undef m_ca_err
#undef m_wf_err
#undef m_cl_err
