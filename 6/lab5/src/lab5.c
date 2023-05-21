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
#include "number_work/recieve_gt0_int_number.h"
#define BUF_SIZE 4096

//  define error messages
#define cfvo_re_err { fprintf(stderr, "%s: Error encountered while matching a char array to a reguar expression.\n", __func__); return -1; }
#define cfvo_nm_err {\
    if (n % 10 == 1 && n % 100 != 11)       fprintf(stderr, "%s: The %lust line is not a supported operation (%s).\n", __func__, n, operation);\
    else if (n % 10 == 2 && n % 100 != 12)  fprintf(stderr, "%s: The %lund line is not a supported operation (%s).\n", __func__, n, operation);\
    else if (n % 10 == 3 && n % 100 != 13)  fprintf(stderr, "%s: The %lurd line is not a supported operation (%s).\n", __func__, n, operation);\
    else                                    fprintf(stderr, "%s: The %luth line is not a supported operation (%s).\n", __func__, n, operation);\
  return -2;\
}

#define rf_m_err { fprintf(stderr, "%s: Failed to allocate memory on the heap.\n", __func__); return -1; }
#define rf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -2; }
#define rf_nel_err { fprintf(stderr, "%s: File does not contain enough lines (has %lu, needs %lu).\n", __func__, count, *n); return -3; }
#define rf_cfvo_err { fprintf(stderr, "%s: Error encountered while checking if a sequence of characters is a valid operation(of type 'a b op').\n", __func__); return -4; }
#define rf_bc_err { fprintf(stderr, "%s: File contains a bad character (%c).\n", __func__, buffer[i]); return -5; }
#define rf_rc_err { fprintf(stderr, "%s: Error encountered while compiling a regular expression. (%s)\n", __func__, pattern); return -6; }

#define po_apf_err { fprintf(stderr, "%s: Error encountered while converting a long int(%ld) into a string.\n", __func__, c); return -1; }
#define po_bo_err { fprintf(stderr, "%s: Recieved operator (%c) is not a supported operation(impossible case).\n", __func__, *op); return -2; }

#define wf_w_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -1; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be %d.\n", __func__, argc - 1, 2); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -2; }
#define m_cr_err { fprintf(stderr, "%s: Error encountered while creating a file.\n", __func__); return -3; }
#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -4; }
#define m_rof_err { fprintf(stderr, "%s: Error encountered while recieving the output file.\n", __func__); return -5; }
#define m_rgin_err { fprintf(stderr, "%s: Error encountered while recieving a number.\n", __func__); return -6; }
#define m_rf_err { fprintf(stderr, "%s: Error encountered while reading the input file.\n", __func__); return -7; }
#define m_po_err { fprintf(stderr, "%s: Error encountered while processing a operation.\n", __func__); return -8; }
#define m_wf_err { fprintf(stderr, "%s: Error encountered while writing into the output file.\n", __func__); return -9; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -10; }




char input_file_name[PATH_MAX];
char output_file_name[PATH_MAX];
int32_t input_fd;
int32_t output_fd;

char* operation;

uint64_t n;
int64_t a;
int64_t b;
char op;

//  -1 - regexec
//  -2 - no match
int32_t check_for_valid_operation(char* operation, regex_t *reg) {
  int32_t ret = regexec(&(*reg), operation, 0, NULL, 0);
  if (ret == 0) {
    //match found, keep going
    return 1;
  }
  else if (ret == REG_NOMATCH) cfvo_nm_err
  else cfvo_re_err
}

//  -1 - malloc
//  -2 - read
//  -3 - not enough lines
//  -4 - check_for_valid_operation
//  -5 - bad char
//  -6 - regcomp
int32_t read_file(int32_t input_descriptor, uint64_t *n, char* *operation) {
  ssize_t bytes_in;
  char buffer[BUF_SIZE];

  ssize_t i;
  uint64_t count = 1;
  uint64_t char_size = 0;
  uint64_t max_size = 1;
  *operation = (char*)malloc(sizeof(char) * max_size);
  char* aux;

  regex_t regex;
  char* pattern = "^[-]?([1-9][0-9]*|0)[ ][-]?([1-9][0-9]*|0)[ ][+-\\*/%]$";
  if (regcomp(&regex, pattern, REG_EXTENDED) != 0) rf_rc_err
  while ((bytes_in = read(input_descriptor, buffer, BUF_SIZE)) > 0 && !(count > *n)) {
    i = 0;
    while (i < bytes_in && !(count > *n)) {
      if (buffer[i] == '\n') {
        ++count;
      }
      else if (count == *n) {
        if ((buffer[i] >= '0' && buffer[i] <= '9') || strchr(" +-*/%", buffer[i])) {
          if (!(buffer[i] == ' ' && ((char_size > 0 && (*operation)[char_size - 1] == ' ') || char_size == 0))) {
            if (!(char_size < max_size - 1)) {
              ++max_size;
              aux = (char*)malloc(sizeof(char) * max_size);
              if (aux == NULL) rf_m_err
              for (uint64_t j = 0; j < char_size; ++j) {
                aux[j] = (*operation)[j];
              }
              free(*operation);
              *operation = aux;
              aux = NULL;
            }
            (*operation)[char_size] = buffer[i];
            ++char_size;
          }
        }
        else rf_bc_err
      }
      ++i;
    }
  }
  if (bytes_in == -1) rf_r_err

  (*operation)[char_size] = '\0';                 //adds '\0' to the minibuffer

  if (count < *n) rf_nel_err
  else {
    if (check_for_valid_operation(*operation, &regex) < 0) rf_cfvo_err
  }

  regfree(&regex);
  return 0;
}

//  -1 - malloc
//  -2 - bad operation
int32_t process_operation(char* *operation, int64_t *a, int64_t *b, char *op) {
  int64_t c;
  char* result;
  char* aux;
  uint64_t i = 0;
  uint64_t j;
  while ((*operation)[i] != ' ') {
    ++i;
  }
  (*operation)[i] = '\0';
  *a = strtoll(*operation, NULL, 10);
  (*operation)[i] = ' ';
  ++i;
  j = i;
  while ((*operation)[j] != ' ') {
    ++j;
  }
  (*operation)[j] = '\0';
  *b = strtoll(*operation + i, NULL, 10);
  (*operation)[j] = ' ';
  ++j;
  *op = (*operation)[j];
  switch (*op) {
    case '+': c = (*a) + (*b); break;
    case '-': c = (*a) - (*b); break;
    case '*': c = (*a) * (*b); break;
    case '/': c = (*a) / (*b); break;
    case '%': c = (*a) % (*b); break;
    default:  po_bo_err        break; //impossible case (how did this get past the regex)
  }
  while (j > i) {
      (*operation)[j] = (*operation)[j - 2];
      --j;
  }
  (*operation)[j] = *op;
  if (asprintf(&result, "%ld", c) == -1) po_apf_err
  aux = (char*)malloc(sizeof(char) * strlen(*operation) + 3 /*" = "*/ + strlen(result) + 1);
  i = 0;
  strcpy(aux, *operation);
  strcat(aux, " = ");
  strcat(aux, result);
  free(result);
  free(*operation);
  *operation = aux;
  aux = NULL;
  return 0;
}

//  -1 - write
int32_t write_file(int32_t output_descriptor, char* operation) {
  char buffer[BUF_SIZE];    //buffer
  
  uint64_t i = 0;           //buffer position
  uint64_t j = 0;           //minibuffer position

    j = 0;
    while (i + j < BUF_SIZE && j < strlen(operation)) {
      buffer[i + j] = operation[j];
      ++j;
    }
    if (i + j < BUF_SIZE && j == strlen(operation)) {     //usually this is '\0' but '\0' is not a good separator
      buffer[i + j] = '\n';                               //also you don't need '\0' when printing something
      ++j;
    }
    i = i + j;

    while (i == BUF_SIZE) {                               //will modify the buffer until it's no longer filled
      if (write(output_descriptor, buffer, BUF_SIZE) == -1) wf_w_err
      i = 0;
      while (j < strlen(operation) && i < BUF_SIZE) {
        buffer[i] = operation[j];
        ++i;
        ++j;
      }
      if (j == strlen(operation) && i < BUF_SIZE) {
        buffer[i] = '\n';
        ++i;
        ++j;
      }
      //same stuff as above but starting from 0 in the buffer and from j in minibuffer
      //this way objects that're either separated in different buffers or are
      //bigger than a normal buffer can be written using write
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
//  -6 - recieve_gt0_int_number
//  -7 - read_file
//  -8 - process_operation
//  -9 - write_file
//  -10 - close
int32_t main(int32_t argc, char** argv) {
//  print_args(argc, argv);
  if (argc - 1 != 3) m_bac_err

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

  if (recieve_gt0_int_number(argc, argv, &n, 3) < 0) m_rgin_err

/*
  if (process_file(input_fd, output_fd) < 0) m_pf_err
*/

  if (read_file(input_fd, &n, &operation) < 0) m_rf_err

  if (process_operation(&operation, &a, &b, &op) < 0) m_po_err

  if (write_file(output_fd, operation) < 0) m_wf_err

  free(operation);

  if(close(input_fd) == -1) m_cl_err

  if(close(output_fd) == -1) m_cl_err

  return 0;
}

#undef cfvo_re_err
#undef cfvo_nm_err

#undef rf_m_err
#undef rf_r_err
#undef rf_nel_err
#undef rf_cfvo_err
#undef rf_bc_err
#undef rf_rc_err

#undef po_apf_err
#undef po_bo_err

#undef wf_w_err

#undef m_bac_err
#undef m_o_err
#undef m_cr_err
#undef m_rif_err
#undef m_rof_err
#undef m_rgin_err
#undef m_rf_err
#undef m_po_err
#undef m_wf_err
#undef m_cl_err
