/*
* Filename: sortare_cu-lacate.c
*
* A doua variantă de sortare a unui fișier, completă, în sensul că folosim lacăte pentru acces exclusiv la secțiunea de fișier modificată la un moment dat.
* Lacătele sunt puse pe secțiuni minimale din fișier (i.e., se blochează din fișier doar atât cât este necesar pentru a face inversiunea unei perechi de întregi)
* și sunt menținute pe durate minimale (i.e., doar atât timp cât este nevoie pentru a face inversiunea unei perechi de întregi).
*
* Ca urmare, soluția este corectă -- în sensul că va funcționa corect întotdeauna, producând rezultatele corecte, chiar și atunci când se lansează în execuție
* simultană mai multe instanțe ale programului (i.e., un job de tip SPMD), pentru a sorta concurent un același fișier de date.
* Cu alte cuvinte, nu se vor obține rezultate incorecte, datorită fenomenelor de data race !!!
*/

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "lab2.h"

int main(int argc,char** argv) {
	if (argc - 1 > 1) { fprintf(stderr, "usage: (-i|-o)?"); return -1; }
  if (argc - 1 == 1) {
    if (strcmp(argv[1], "-i") == 0) {
      if (init_numbers() < 0) { fprintf(stderr, "in: initialization failed\n"); return -1; }
    }
    else if (strcmp(argv[1], "-o") == 0) {
      if (print_numbers() < 0) { fprintf(stderr, "pn: failed to print\n"); return -1; }
    }
    else { fprintf(stderr, "main: bad option\n") ; return -2; }
  }
	else {
    if (sort_numbers() < 0) { fprintf(stderr, "sn: failed to sort\n"); return -1; }
  }
	return 0;
}

int32_t init_numbers() {
	int32_t fd;
	uint64_t i, count;
  int64_t number;
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
	printf("Insert the number of numbers you want to add (must be a positive integer): ");
	while (scanf("%lu", &count) != 1) {
    printf("You haven't introduced a valid positive integer, try again: ");
  }
  lock.l_len = count * sizeof(int64_t);
	if ((fd = creat("numbers.bin", 0600)) == -1) { perror("creat"); return -1; }
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("lock"); return -1; }
  if (ftruncate(fd, count * sizeof(int64_t)) == -1) { perror("ftruncate"); return -1; }
  i = 0;
  while (i < count) {
    printf("Insert v[%lu] here (must be an integer): ", i);
    while (scanf("%ld", &number) != 1) {
      printf("You haven't introduced a valid integer, try again: ");
    }
    if (write(fd, &number, sizeof(int64_t)) == -1) { perror("write"); return -1; }
    ++i;
  }
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("unlock"); return -1; }
	if (close(fd) == -1) { perror("close"); return -1; }
  return 0;
}
int32_t print_numbers() {
	int32_t fd, read_code;
	int64_t number;
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
	if ((fd = open("numbers.bin", O_RDONLY), 0600) == -1) { perror("open"); return -1; }
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("lock"); return -1; }
	while ((read_code = read(fd, &number, sizeof(int64_t))) > 0) {
    printf("%ld ", number);
  }
  if (read_code == -1) { perror("read"); return -1; }
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("unlock"); return -1; }
  printf("\n");
	if (close(fd) == -1) { perror("close"); return -1; }
  return 0;
}
int32_t sort_numbers() {
  int32_t fd;
  int64_t number_1, number_2, aux;
  struct stat s;
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = sizeof(int64_t);
  struct flock unlock;
  unlock.l_type = F_UNLCK;
  unlock.l_whence = SEEK_SET;
  unlock.l_start = 0;
  unlock.l_len = sizeof(int64_t);
  uint64_t i, j;
  if ((fd = open("numbers.bin", O_RDWR), 0600) == -1) { perror("open"); return -1; }
  if (stat("numbers.bin", &s) == -1) { perror("stat"); return -1; }
  i = 0;
  while (i < s.st_size - sizeof(int64_t)) {
    lock.l_start = i;
    if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("frst number fcntl lock"); return -1; }
    //printf("Placing a lock\tfrom %lu to %lu\n", i, i + sizeof(int64_t));
    j = i + sizeof(int64_t);
    while (j < (uint64_t)s.st_size) {
      //printf("Step:\t%lu : %lu\n\n", i / sizeof(int64_t), j / sizeof(int64_t));
      lock.l_start = j;
      if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("second number fcntl lock"); return -1; }
      if (lseek(fd, i, SEEK_SET) == -1) { perror("first number read lseek"); return -1; }
      if (read(fd, &number_1, sizeof(int64_t)) == -1) { perror("first number read"); return -1; }
      if (lseek(fd, j, SEEK_SET) == -1) { perror("second number read lseek"); return -1; }
      if (read(fd, &number_2, sizeof(int64_t)) == -1) { perror("second number read"); return -1; }
      //printf("Placing a lock\tfrom %lu to %lu\n", j, j + sizeof(int64_t));
      //printf("Checking: v[%lu] = %ld and v[%lu] = %ld\n", i / sizeof(int64_t), number_1, j / sizeof(int64_t), number_2);
      if (number_1 > number_2) {
        //printf("Changing: v[%lu] = %ld and v[%lu] = %ld\n", i / sizeof(int64_t), number_1, j / sizeof(int64_t), number_2);
        if (lseek(fd, i, SEEK_SET) == -1) { perror("second number swap lseek"); return -1; }
        if (write(fd, &number_2, sizeof(int64_t)) == -1) { perror("second number write"); return -1; }
        if (lseek(fd, j, SEEK_SET) == -1) { perror("first number swap lseek"); return -1; }
        if (write(fd, &number_1, sizeof(int64_t)) == -1) { perror("first number write"); return -1; }
        aux = number_1;
        number_1 = number_2;
        number_2 = aux;
        //printf("v[%lu] = %ld and v[%lu] = %ld have been changed\n", i / sizeof(int64_t), number_1, j / sizeof(int64_t), number_2);
      }
      unlock.l_start = j;
      if (fcntl(fd, F_SETLKW, &unlock) == -1) { perror("second number fcntl unlock"); return -1; }
      //printf("Unlocking\tfrom %lu to %lu\n", j, j + sizeof(int64_t));
      j = j + sizeof(int64_t);
    }
    unlock.l_start = i;
    if (fcntl(fd, F_SETLKW, &unlock) == -1) { perror("first number fcntl unlock"); return -1; }
    //printf("Unlocking\tfrom %lu to %lu\n", i, i + sizeof(int64_t));
    i = i + sizeof(int64_t);
  }
  if (close(fd) == -1) { perror("close"); return -1; }
	printf("[PID: %d] This process has stopped processing stuff.\n", getpid());
  return 0;
}
