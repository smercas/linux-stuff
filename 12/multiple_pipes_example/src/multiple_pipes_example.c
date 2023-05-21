#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <wait.h>

int32_t master_work(int32_t**, int32_t, uint64_t, int32_t*);
int32_t slave_work(int32_t, int32_t);

int32_t main() {
  uint64_t n;
  uint64_t i, j;
  int32_t* pids;
  int32_t** pipes;
  int32_t main_pipe[2];
  if (pipe(main_pipe) == -1) { perror("pipe"); return -1; }
  n = 3;
  if ((pipes = (int32_t**)malloc(sizeof(int32_t*) * n)) == NULL) { perror("malloc"); return -1; }
  if ((pids = (int32_t*)malloc(sizeof(int32_t) * n)) == NULL) { perror("malloc"); return -1; }
  i = 0;
  while (i < n) {
    ++i;
  }
  i = 0;
  while (i < n) {
    if ((pipes[i] = (int32_t*)malloc(sizeof(int32_t) * 2)) == NULL) { perror("malloc"); return -1; }
    if (pipe(pipes[i]) == -1) { perror("pipe"); return -1; }
    switch (pids[i] = fork()) {
      case -1:
        perror("fork");
        return -1;
        break;
      case 0:
        j = 0;
        while (j < i) {
          if (close(pipes[j][0]) == -1) { perror("close"); return -1; }
          if (close(pipes[j][1]) == -1) { perror("close"); return -1; }
          ++j;
        }
        if (close(pipes[i][1]) == -1) { perror("close"); return -1; }
        if (close(main_pipe[0]) == -1) { perror("close"); return -1; }
        slave_work(pipes[i][0], main_pipe[1]);
        return 0;
        break;
      default:
        break;
    }
    ++i;
  }
  i = 0;
  while (i < n) {
    if (close(pipes[i][0]) == -1) { perror("close"); return -1; }
    ++i;
  }
  if (close (main_pipe[1]) == -1) { perror("close"); return -1; }
  master_work(pipes, main_pipe[0], n, pids);
  i = 0;
  while (i < n) {
    free(pipes[i]);
    ++i;
  }
  free(pipes);
  free(pids);
  return 0;
}
int32_t master_work(int32_t** input_to_split, int32_t recieved_input, uint64_t n, int32_t* pids) {
  uint64_t it;
  int32_t nr;
  int32_t sp, s;
  s = 0;
  printf("Introduce some numbers, or 0 to terminate:\n");
  it = 0;
  nr = -1;
  while (nr != 0) {
    scanf("%d", &nr);
    if (nr == 0) break;
    write(input_to_split[it][1], &nr, sizeof(int32_t));
    it = (it + 1) % n;
  }
  it = 0;
  while (it < n) {
    if (close(input_to_split[it][1]) == -1) { perror("close"); return -1; }
    if (waitpid(pids[it], NULL, 0) == -1) { perror("waitpid"); return -1; }
    if (read(recieved_input, &sp, sizeof(int32_t)) != sizeof(int32_t)) { perror("read"); return -1; }
    s = s + sp;
    ++it;
  }
  if (close(recieved_input) == -1) { perror("close"); return -1; }
  printf("%d\n", s);
  return 0;
}
int32_t slave_work(int32_t i, int32_t o) {
  int32_t nr, cod_r, partial_sum = 0;
  do {
    switch (cod_r = read(i, &nr, sizeof(int32_t))) {
      case -1:
        perror("read");
        return -1;
        break;
      case 0:
        break;
      case sizeof(int32_t):
        partial_sum = partial_sum + nr;
        break;
      default:
        fprintf(stderr, "partial read (%d) occured for some god forsaken reason =)))", cod_r);
        return -1;
        break;
    }
  } while (cod_r != 0);
  if (close(i) == -1) { perror("close"); return -1; }
  if (write(o, &partial_sum, sizeof(int32_t)) == -1) { perror("write"); return -1; }
  if (close(o) == -1) { perror("close"); return -1; }
  printf("Partial sum: %d\n", partial_sum);
  return 0;
}
