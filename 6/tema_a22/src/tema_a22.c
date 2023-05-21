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
#include "file_work/is_regular_file.h"
#define BUF_SIZE 5

//  define error messages
#define ronp_ii_err { fprintf(stderr, "%s: Recieved optional argument number should be an integer with values between 1 and %d when it's %d.\n", __func__, c - 2, a); return -1; }
#define ronp_rc_err { fprintf(stderr, "%s: Error encountered while compiling a regular expression. (%s)\n", __func__, pattern); return -2; }
#define ronp_re_err { fprintf(stderr, "%s: Error encountered while matching a char array to a reguar expression.\n", __func__); return -3; }

#define rif_ii_err { fprintf(stderr, "%s: Recieved optional argument number should be an integer with values between 1 and %d when it's %d.\n", __func__, c - 1, o); return -1; }
#define rif_r_err { fprintf(stderr, "%s: Error encountered while solving the path of '%s'.\n", __func__, v[o]); return -2; }
#define rif_irf_err { fprintf(stderr, "%s: Error encountered while trying to find out if %s is a regular file or not.\n", __func__, path); return -3; }

#define rf_r_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -1; }
#define rf_ls_err { fprintf(stderr, "%s: Error encountered while readjusting the file cursor.\n", __func__); return -2; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be either %d or %d.\n", __func__, argc - 1, 1, 2); return -1; }
#define m_o_err { fprintf(stderr, "%s: Error encountered while opening a file.\n", __func__); return -2; }
#define m_rif_err { fprintf(stderr, "%s: Error encountered while recieving the input file.\n", __func__); return -3; }
#define m_ronp_err { fprintf(stderr, "%s: Error encountered while recieving opt-number pairs.\n", __func__); return -4; }
#define m_rf_err { fprintf(stderr, "%s: Error encountered while reading a file.\n", __func__); return -5; }
#define m_cl_err { fprintf(stderr, "%s: Error encountered while closing a file.\n", __func__); return -6; }

char input_file_name[PATH_MAX];
int32_t input_fd;
uint64_t number;
uint64_t file_count;
char mode;
#define M_LINES 1
#define M_LINESM 2
#define M_BYTES 4
#define M_BYTESM 8

//  -1 - no match
//  -0 - match
//  -1 - invalid index
//  -2 - regcomp
//  -3 - regexec
int32_t recieve_opt_number_pair(int32_t c, char** v, char *m, uint64_t *n, int32_t a) {
  if (a <= 0 || a + 1 >= c) ronp_ii_err
  regex_t reg;
  int32_t ret;
  char* pattern = "^[-]?[1-9][0-9]*$";
  if (regcomp(&reg, pattern, REG_EXTENDED) != 0) ronp_rc_err
  ret = regexec(&reg, v[a + 1], 0, NULL, 0);
  regfree(&reg);
  if (ret == REG_NOMATCH) {
    return 1;
  }
  else if (ret != 0) ronp_re_err
  else {
    if (!(strlen(v[a]) >= 2 && v[a][0] == '-' && strchr("nc", v[a][1]))) {
      return 1;
    }
    else {
      switch (v[a][1]) {
        case 'n': *m = *m | M_LINES; break;
        case 'c': *m = *m | M_BYTES; break;
      }
      if (v[a + 1][0] == '-') {
        *m = *m << 1;
      }
      *n = strtoull((v[a + 1][0] == '-' ? v[a + 1] + 1 : v[a + 1]), NULL, 10);
      return 0;
    }
  }
}

//   1 - is a valid input file
//   0 - is not a valid input file
//  -1 - invalid index
//  -2 - realpath
//  -3 - is_regular_file
int32_t recieve_input_file(int32_t c, char** v, char *path, int32_t o) {
  if (o <= 0 || o >= c) rif_ii_err
  if (v[o][0] == '-' || (o - 1 > 0 && v[o - 1][0] == '-' && (v[o][0] == '-' || (v[o][0] >= '0' && v[o][0] <= '9')))) {
    return 0;
  }
  else {
    if (realpath(v[o], path) == NULL) rif_r_err
    int32_t e = (access(path, F_OK) == 0);
    int32_t r = (access(path, R_OK) == 0);
    int32_t f = is_regular_file(path);
    if (f < 0) rif_irf_err
    if (!e || !r || !f) {
      return 0;
    }
    else {
      return 1;
    }
  }
}

//  -1 - read
//  -2 - lseek
int32_t read_file(int32_t input_descriptor, char m, uint64_t n) {
  ssize_t bytes_in;
  char buffer[BUF_SIZE];
  int64_t i;
  uint64_t count = 0;

  if (m & M_BYTESM || m & M_LINESM) {
    uint64_t max_count = 0;
    while ((bytes_in  = read(input_descriptor, buffer, BUF_SIZE)) > 0) {
      i = 0;
      while (i < bytes_in) {
        if (m & M_LINESM) {
          if (buffer[i] == '\n') {
            ++max_count;
          }
        }
        else if (m & M_BYTESM) {
          ++max_count;
        }
        ++i;
      }
    }
    if (bytes_in == -1) rf_r_err

    if (lseek(input_descriptor, 0, SEEK_SET) == -1) rf_ls_err

    while ((bytes_in  = read(input_descriptor, buffer, BUF_SIZE)) > 0 && count < max_count - n) {
      i = 0;
      while (i < bytes_in) {
        if (m & M_LINESM) {
          if (buffer[i] == '\n') {
            ++count;
          }
        }
        else if (m & M_BYTESM) {
          ++count;
        }
        ++i;
        if (count == max_count - n) {
          printf("%.*s", (int)i, buffer);
          i = bytes_in;
        }
      }
      if (count < max_count - n) {
        printf("%.*s", (int)bytes_in, buffer);
      }
    }
    if (bytes_in == -1) rf_r_err
  }
  else if (m & M_BYTES || m & M_LINES) {
    while ((bytes_in  = read(input_descriptor, buffer, BUF_SIZE)) > 0 && count < n) {
      i = 0;
      while (i < bytes_in) {
        if (m & M_LINES) {
          if (buffer[i] == '\n') {
            ++count;
          }
        }
        else if (m & M_BYTES) {
          ++count;
        }
        if (count == n) {
          printf("%.*s", (int)i, buffer);
          i = bytes_in - 1;
        }
        ++i;
      }
      if (count < n) {
        printf("%.*s", (int)bytes_in, buffer);
      }
    }
    if (bytes_in == -1) rf_r_err

  }
  printf("\n\n");
  return 0;
}

//  -1 - bad argument count
//  -2 - open
//  -3 - recieve_input_file
//  -4 - recieve_opt_number_pair
//  -5 - read_file
//  -6 - close
int32_t main(int32_t argc, char** argv) {
//  print_args(argc, argv);

  if (!(argc - 1 >= 1)) m_bac_err

  int64_t i = argc - 2;
  int32_t x;
  while (i > 0 && (x = recieve_opt_number_pair(argc, argv, &mode, &number, i)) > 0) {
    --i;
  }
  if (x != 0) {
    if (i == 0) {
      mode = M_LINES;
      number = 10;
    }
    else m_ronp_err
  }

  //printf("%d : %lu\n", mode, number);

  i = 1;
  while (i < argc && (x = recieve_input_file(argc, argv, input_file_name, i)) >= 0) {
    file_count = file_count + x;
    ++i;
  }
  if (x < 0) m_rif_err

  i = 1;
  if (file_count == 1) {
    while (i < argc && (x = recieve_input_file(argc, argv, input_file_name, i)) == 0) {
      ++i;
    }
    if (x == 1) {
      if ((input_fd = open(input_file_name, O_RDONLY, 0400)) == -1) m_o_err
      if (read_file(input_fd, mode, number) < 0) m_rf_err
      if (close(input_fd) == -1) m_cl_err
    }
    else if (x < 0) m_rif_err
  }
  else {
    while (i < argc && (x = recieve_input_file(argc, argv, input_file_name, i)) >= 0) {
      if (x == 1) {
        printf("==> %s <==\n", input_file_name);
        if ((input_fd = open(input_file_name, O_RDONLY, 0400)) == -1) m_o_err
        if (read_file(input_fd, mode, number) < 0) m_rf_err
        if (close(input_fd) == -1) m_cl_err
      }
      ++i;
    }
    if (x < 0) m_rif_err
  }

  return 0;
}

#undef ronp_ii_err
#undef ronp_rc_err
#undef ronp_re_err

#undef rif_ii_err
#undef rif_r_err
#undef rif_irf_err

#undef pf_r_err
#undef pf_ls_err

#undef m_bac_err
#undef m_o_err
#undef m_rif_err
#undef m_ronp_err
#undef m_rf_err
#undef m_cl_err
