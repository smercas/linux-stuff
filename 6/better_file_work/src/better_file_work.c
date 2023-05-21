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
#include <regex.h>

#include "print_args.h"
#include "file_work/recieve_input_file.h"
#include "file_work/recieve_output_file.h"
#define BUF_SIZE 4096

//  define error messages
#define pf_bw_err { fprintf(stderr, "%s: Fatal write error, the number of bytes that ahve been read (%ld) and the number of bytes that have been written (%ld) are not equal.\n", __func__, bytes_in, bytes_out); return -1; }
#define pf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -2; }
#define pf_w_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -3; }

#define cfvw_re_err { fprintf(stderr, "%s: Error encountered while matching a char array to a reguar expression.\n", __func__); return -1; }
#define cfvw_nm_err { fprintf(stderr, "%s: File contains something that is not a word (%s).\n", __func__, mb); return -2; }

#define aw_m_err { fprintf(stderr, "%s: Failed to allocate memory on the heap.\n", __func__); return -1; }

#define rf_m_err { fprintf(stderr, "%s: Failed to allocate memory on the heap.\n", __func__); return -1; }
#define rf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -2; }
#define rf_cfvw_err { fprintf(stderr, "%s: Error encountered while checking if a sequence of characters is a valid word.\n", __func__); return -3; }
#define rf_bc_err { fprintf(stderr, "%s: File contains a bad character (%c).\n", __func__, buffer[i]); return -4; }
#define rf_ls_err { fprintf(stderr, "%s: Error encountered while readjusting the file cursor.\n", __func__); return -5; }
#define rf_aw_err { fprintf(stderr, "%s: Error encountered while adding a word into the words list.\n", __func__); return -6; }
#define rf_rc_err { fprintf(stderr, "%s: Error encountered while compiling a regular expression. (%s)\n", __func__, pattern); return -7; }

#define wf_m_err { fprintf(stderr, "%s: Failed to allocate memory on the heap.\n", __func__); return -1; }
#define wf_w_err { fprintf(stderr, "%s: Error encountered while writing into a file.\n", __func__); return -2; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be %d.\n", __func__, argc - 1, 2); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -2; }
#define m_cr_err { fprintf(stderr, "%s: Error encountered while creating a file.\n", __func__); return -3; }
#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -4; }
#define m_rof_err { fprintf(stderr, "%s: Error encountered while recieving the output file.\n", __func__); return -5; }
#define m_rf_err { fprintf(stderr, "%s: Error encountered while reading the input file.\n", __func__); return -6; }
#define m_pf_err { fprintf(stderr, "%s: Error encountered while processing files.\n", __func__); return -7; }
#define m_wf_err { fprintf(stderr, "%s: Error encountered while writing into the output file.\n", __func__); return -8; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -9; }

char input_file_name[PATH_MAX];
char output_file_name[PATH_MAX];
int32_t input_fd;
int32_t output_fd;

char** words;
uint64_t word_count;

//basically copies the input file into the output file
//  -1 -bad write (impossible case)
//  -2 -read
//  -3 -write
int32_t process_file(int32_t input_descriptor, int32_t output_descriptor) {
  ssize_t bytes_in;
  ssize_t bytes_out;
  char buffer[BUF_SIZE];

  while ((bytes_in  = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    bytes_out = write(output_descriptor, buffer, bytes_in);
    if (bytes_out != bytes_in) pf_bw_err
    if (bytes_out == -1) pf_w_err
  }
  if (bytes_in == -1) pf_r_err
  return 0;
}
//this is some of the more basic stuff

//  -1 - regexec
//  -2 - no match
int32_t check_for_valid_word(char* mb, uint64_t *size, uint64_t *count, regex_t *reg) {
  if ((*size) > 0) {
    int32_t ret = regexec(&(*reg), mb, 0, NULL, 0);
    if (ret == 0) {
      //match found, keep going
    }
    else if (ret == REG_NOMATCH) cfvw_nm_err
    else cfvw_re_err    
    (*size) = 0;
    ++(*count);
    return 1;
  }
  return 0;
}

//  -1 - malloc
int32_t add_word(char* mb, uint64_t *size, char** *iv, uint64_t *ic) {
  if ((*size) > 0) {
    mb[*size] = '\0';
    (*iv)[*ic] = (char*)malloc(sizeof(char) * (strlen(mb) + 1));
    if ((*iv)[*ic] == NULL) aw_m_err
    strcpy((*iv)[*ic], mb);               //these lines store the
    (*size) = 0;                          //objects into the object
    ++(*ic);                              //array, replace them
    return 1;                             //with whatever you want
  }
  return 0;
}

//  -1 - malloc
//  -2 - read
//  -3 - check_for_valid_word
//  -4 - bad char
//  -5 - lseek
//  -6 - add_word
//  -7 - regcomp
int32_t read_file(int32_t input_descriptor, char** *input_values, uint64_t *input_count) {
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
  char* pattern = "^[a-zA-Z]+$";
  int32_t regret = regcomp(&regex, pattern, REG_EXTENDED);
  if (regret != 0) rf_rc_err

  while ((bytes_in = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    i = 0;
    while (i < bytes_in) {
      if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z')) {
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
        

        if (check_for_valid_word(minibuffer, &current_char_size, &*input_count, &regex) < 0) rf_cfvw_err
      }
      else rf_bc_err
      ++i;
    }
  }
  if (bytes_in == -1) rf_r_err
  if (check_for_valid_word(minibuffer, &current_char_size, &*input_count, &regex) < 0) rf_cfvw_err

  regfree(&regex);

  //if the function hasn't ended yet, then all the objects that are in the input file are correct
  
  if (lseek(input_descriptor, 0, SEEK_SET) == -1) rf_ls_err

  *input_values = (char**)malloc(sizeof(char*) * (*input_count));
  if (*input_values == NULL) rf_m_err

  (*input_count) = 0;

  //the second while is used to actually read and store the objects

  while ((bytes_in = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
    i = 0;
    while (i < bytes_in) {
      if ((buffer[i] >= 'a' && buffer[i] <= 'z') || (buffer[i] >= 'A' && buffer[i] <= 'Z')) {
        minibuffer[current_char_size] = buffer[i];
        ++current_char_size;
      }
      else if (buffer[i] == ' ' || buffer[i] == '\n') {
        if (add_word(minibuffer, &current_char_size, &*input_values, &*input_count) < 0) rf_aw_err
      }
      ++i;
    }
  }
  if (bytes_in == -1) rf_r_err
  if (add_word(minibuffer, &current_char_size, &*input_values, &*input_count) < 0) rf_aw_err
  free (minibuffer);
  return 0;
}

//  -1 - malloc
//  -2 - write
int32_t write_file(int32_t output_descriptor, char** output_values, uint64_t output_count) {
  char buffer[BUF_SIZE];    //buffer
  char* minibuffer;         //buffer for the object that'll have to be written
  
  uint64_t count = 0;       //number of objects that have been written
  uint64_t i = 0;           //buffer position
  uint64_t j = 0;           //minibuffer position

  while (count < output_count) {
    minibuffer = (char*)malloc(strlen(output_values[count]) + 1);   //these two lines are a placeholder for the minibuffer
    if (minibuffer == NULL) wf_m_err                    //initialisation, if no preprocessing is required, skip
    strcpy(minibuffer, output_values[count]);                       //this, otherwise do the preprocessing ur own way
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
      while (j < strlen(minibuffer) && i < BUF_SIZE) {
        buffer[i] = minibuffer[j];
        ++i;
        ++j;
      }
      if (j == strlen(minibuffer) && i < BUF_SIZE) {
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
//  -7 - process_file
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

  if (read_file(input_fd, &words, &word_count) < 0) m_rf_err

  uint64_t i = 0;
/*
  while (i < word_count) {
    printf("%lu: %s\n", i, words[i]);
    ++i;
  }
  printf("\n");
*/

  if (write_file(output_fd, words, word_count) < 0) m_wf_err

  i = 0;
  while (i < word_count) {
    free(words[i]);
    ++i;
  }
  free(words);

  if (close(input_fd) == -1) m_cl_err

  if (close(output_fd) == -1) m_cl_err

  return 0;
}

#undef pf_bw_err
#undef pf_r_err
#undef pf_w_err

#undef cfvw_re_err
#undef cfvw_nm_err

#undef aw_m_err

#undef rf_m_err
#undef rf_r_err
#undef rf_cfvw_err
#undef rf_bc_err
#undef rf_ls_err
#undef rf_aw_err
#undef rf_rc_err

#undef wf_m_err
#undef wf_w_err

#undef m_bac_err
#undef m_o_err
#undef m_cr_err
#undef m_rif_err
#undef m_rof_err
#undef m_rf_err
#undef m_pf_err
#undef m_wf_err
#undef m_cl_err
