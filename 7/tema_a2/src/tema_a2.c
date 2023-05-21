//  TODO: respectivele omisiuni vă rămân ca temă să le completați dvs. !!!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include "tema_a2.h"

int32_t main(int32_t argc, char** argv) {
	if (argc - 1 == 0) { fprintf(stderr, "usage: -o|(id change )+"); return -1; }
	else if (strcmp(argv[1], "-o") == 0) {
		if (print_bank_accounts() < 0) { fprintf(stderr, "pba: failed to print\n"); return -1; }
	}
	else {
    if (update_bank_accounts(argc, argv) < 0) { fprintf(stderr, "uba: failed to update the bank accounts"); return -1; }
  }
	return 0;
}

int32_t print_bank_accounts() {
	int32_t fd, read_code_1, read_code_2;
  uint64_t id;
	double balance;
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
	if ((fd = open("bank_accounts.bin", O_RDONLY | O_CREAT, 0666)) == -1) { perror("open"); return -1; }
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("lock"); return -1; }
	while ((read_code_1 = read(fd, &id, sizeof(int64_t))) > 0) {
    printf("%lu:\t", id);
    if ((read_code_2 = read(fd, &balance, sizeof(double))) > 0) {
      printf("%.2lf\n", balance);
    }
    else if (read_code_2 == 0) {
      printf("________\n");
    }
    else if (read_code_2 == -1) { perror("second read"); return -1; }
  }
  if (read_code_1 == -1) { perror("first read"); return -1; }
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("unlock"); return -1; }
	if (close(fd) == -1) { perror("close"); return -1; }
  return 0;
}
int32_t update_bank_accounts(int32_t n, char** nr) {
	uint64_t id, rid;
  double balance, change;
	int32_t fd, i, read_code_1, read_code_2;
  struct flock update_lock;
  update_lock.l_type = F_WRLCK;
  update_lock.l_whence = SEEK_CUR;
  update_lock.l_start = 0;
  update_lock.l_len = sizeof(double);
  struct flock update_unlock;
  update_unlock.l_type = F_UNLCK;
  update_unlock.l_whence = SEEK_CUR;
  update_unlock.l_start = -sizeof(double);
  update_unlock.l_len = sizeof(double);
  struct flock add_lock;
  add_lock.l_type = F_WRLCK;
  add_lock.l_whence = SEEK_CUR;
  add_lock.l_start = 0;
  add_lock.l_len = sizeof(uint64_t) + sizeof(double);
  struct flock add_unlock;
  add_unlock.l_type = F_WRLCK;
  add_unlock.l_whence = SEEK_CUR;
  add_unlock.l_start = -(sizeof(uint64_t) + sizeof(double));
  add_unlock.l_len = sizeof(uint64_t) + sizeof(double);
	if ((fd = open("bank_accounts.bin", O_RDWR | O_CREAT, 0600)) == -1) return -1;
  i = 1;
	while (i < n) {
		if (sscanf(nr[i], "%lu", &rid) != 1 || sscanf(nr[i + 1], "%lf", &change) != 1) {
      fprintf(stderr, "You've introduced a bad value as input(%s)(%s).\n", nr[i], nr[i + 1]);
      return -2;
    }
    while ((read_code_1 = read(fd, &id, sizeof(uint64_t))) >= 0) {
      if (read_code_1 > 0) {
        if (rid == id) {
          if (fcntl(fd, F_SETLKW, &update_lock) == -1) { perror("update lock"); return -1; }
          printf("Placing an update lock\t\tfrom %lu to %lu\n", lseek(fd, 0, SEEK_CUR), lseek(fd, 0, SEEK_CUR) + sizeof(double));
          if ((read_code_2 = read(fd, &balance, sizeof(double))) > 0) {
            if (balance + change < 0) {
              fprintf(stderr, "Current operation cannot be performed (balance = %f < sum_to_be_subtracted = %f) \
so it won't be performed.\n", balance, -change);
            }
            else {
              printf("id %*lu: %.2lf => %.2lf\n", 10, id, balance, balance + change);
              if (lseek(fd, -sizeof(double), SEEK_CUR) == -1) { perror("update lseek"); return -1; }
              balance = balance + change;
              if (write(fd, &balance, sizeof(double)) == -1) { perror("update write"); return -1; }
            }
            if (fcntl(fd, F_SETLKW, &update_unlock) == -1) { perror("update unlock"); return -1; }
            printf("Placing an update unlock\tfrom %lu to %lu\n", lseek(fd, 0, SEEK_CUR) - sizeof(double), lseek(fd, 0, SEEK_CUR));
            break;
          }
          else if (read_code_2 == 0) { fprintf(stderr, "Id %lu does not have a balance. How did we get here???\n", id); return -1; }
          else if (read_code_2 == -1) { perror("second read"); return -1; }
        }
        else {
          if (lseek(fd, sizeof(double), SEEK_CUR) == -1) { perror("id mismatch lseek"); return -1; }
        }
      }
      else {
        if (fcntl(fd, F_SETLKW, &add_lock) == -1) { perror("add lock"); return -1; }
        printf("Placing an add lock\t\tfrom %lu to %lu\n", lseek(fd, 0, SEEK_CUR), lseek(fd, 0, SEEK_CUR) + sizeof(uint64_t) + sizeof(double));
        if ((read_code_2 = read(fd, &id, sizeof(uint64_t))) > 0) {
          add_unlock.l_start = add_unlock.l_start + sizeof(double);
          if (fcntl(fd, F_SETLKW, &add_unlock) == -1) { perror("bad add lock removal"); return -1; }
        printf("Removing a bad add lock\tfrom %lu to %lu\n", lseek(fd, 0, SEEK_CUR) - sizeof(double), lseek(fd, 0, SEEK_CUR));
          add_unlock.l_start = add_unlock.l_start - sizeof(double);
        }
        else if (read_code_2 == 0) {
          if (change < 0) {
            fprintf(stderr, "Current operation cannot be performed (id = %lu, starting_sum = %f < 0) \
so it won't be performed.\n", rid, change);
          }
          else {
            printf("id %*lu: %.2lf\n", 10, rid, change);
            if (write(fd, &rid, sizeof(uint64_t)) == -1) { perror("add write id"); return -1; }
            if (write(fd, &change, sizeof(double)) == -1) { perror("add write balance"); return -1;}
            if (fcntl(fd, F_SETLKW, &add_unlock) == -1) { perror("add unlock"); return -1; }
            printf("Placing an add unlock\t\tfrom %lu to %lu\n", lseek(fd, 0, SEEK_CUR) - sizeof(uint64_t) - sizeof(double), lseek(fd, 0, SEEK_CUR));
          }
          break;
        }
      }
    }
    if (read_code_1 == -1) { perror("first read"); return -1; }
    i = i + 2;
    if (lseek(fd, 0, SEEK_SET) == -1) { perror("while reset lseek"); return -1; }
  }
	if (close(fd) == -1) return -1;
	printf("[PID: %d] This process has stopped processing stuff.\n", getpid());
  return 0;
}
