/*
  Program: șablonul de creare a N fii ai procesului curent.
*/
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <math.h>

#define rgin_rc_err { fprintf(stderr, "%s: Error encountered while compiling a regular expression. (%s)\n", __func__, pattern); return -1; }
#define rgin_re_err { fprintf(stderr, "%s: Error encountered while matching a char array to a reguar expression.\n", __func__); return -2; }
#define rgin_nan_err { fprintf(stderr, "%s: The recieved input string '%s' does not qualify as a valid number (must be an greater than 0 integer).\n", __func__, v); return -3; }

#define r_m_err { fprintf(stderr, "%s: Failed to allocate memory on the heap.\n", __func__); return -1; }
#define r_f_err { fprintf(stderr, "%s: Error encountered while creating a child process.\n", __func__); return -2; }
#define r_w_err { fprintf(stderr, "%s: Error encountered while waiting for a child process to end.\n", __func__); return -3; }

#define m_bac_err { fprintf(stderr, "%s: The number of arguments that have been passed is %d when it should be less than %d.\n", __func__, argc - 1, 2); return -1; }
#define m_rgin_err { fprintf(stderr, "%s: Error encountered while recieving an input number (%s).\n", __func__, number); return -2; }
#define m_r_err { fprintf(stderr, "%s: Error encountered while doing recursion.\n", __func__); return -3; }

//  -1 regcomp
//  -2 regexec
//  -3 not a number
int32_t recieve_gt0_int_number(char* v, uint64_t *n) {
  regex_t reg;
  int32_t ret;
  char* pattern = "^[1-9][0-9]*$";
  if (regcomp(&reg, pattern, REG_EXTENDED) != 0) rgin_rc_err

  if ((ret = regexec(&reg, v, 0, NULL, 0)) == _REG_NOMATCH) rgin_nan_err
  regfree(&reg);
  if (ret != 0) rgin_re_err
  *n = strtoull(v, NULL, 10);
  return 0;
}

//  -1 - malloc
//  -2 - fork
//  -3 - wait
int32_t rec (uint64_t k, uint64_t n) {
  uint64_t i, j;
  pid_t pid;
  pid_t** child = (pid_t**)malloc(sizeof(pid_t*) * n);
  if (child == NULL) r_m_err
  int32_t** code_child = (int32_t**)malloc(sizeof(int32_t*) * n);
  j = k;
  for (i = 0; i < n; ++i) {
    child[i] = (pid_t*)malloc(sizeof(pid_t) * j);
    child[i] = (int32_t*)malloc(sizeof(int32_t) * j);
    j = j * k;
  }



  if (code_child == NULL) r_m_err
  for (i = 0; i < k; ++i) {
      switch (pid = fork()) {
        case -1:
          r_f_err
          break;
        case  0:
          printf("Child process number %lu (PID = %d, parent process' PID = %d).\n", j + 1, getpid(), getppid());
          if (n == 0) {
            return i;
          }
          else {
            --n;
            i = 0;
            break;
          }
        default:
          child[i] = pid;
          break;
      }
    }
  //  now we wait
  for (i = 0; i < k; ++i) {
    if (wait(&code_child[i]) == -1) r_w_err
  }
  printf("(1, 1): PID=%d PPID=%d\n", getpid(), getppid());
  for (i = 0; i < k; ++i) {
    printf("child %lu: %d termination_code=%d\n", i + 1, child[i], code_child[i]);
  }
  free(child);
  free(code_child);
  return 0;
}

//  -1 - bad argument count
//  -2 - recieve_gt0_int_number
//  -3 - rec
int32_t main(int32_t argc, char* argv[])
{
  char number[128];
  int32_t code;
  uint64_t n, k;

  if (argc - 1 >= 3) m_bac_err

  if (argc - 1 >= 2) strcpy(number, argv[2]);

  while ((code = recieve_gt0_int_number(number, &n)) == -3) {
    printf("insert another number here (remember, it must be an integer that's greater than zero): ");
    scanf("%s", number);
  }
  if (code < 0) m_rgin_err

  if (argc - 1 >= 1) strcpy(number, argv[1]);

  while ((code = recieve_gt0_int_number(number, &k)) == -3) {
    printf("insert another number here (remember, it must be an integer that's greater than zero): ");
    scanf("%s", number);
  }
  if (code < 0) m_rgin_err

  printf("Initial process PID: %d, parent process' PID: %d.\n", getpid(), getppid());

  if (rec(k, n) < 0) 

  return 0;
}

#undef rgin_rc_err
#undef rgin_re_err
#undef rgin_nan_err

#undef m_bac_err
#undef m_rgin_err
#undef m_f_err
#undef m_w_err
