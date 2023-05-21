//  TODO: respectivele omisiuni vă rămân ca temă să le completați dvs. !!!
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdint.h>
#include "lab1.h"

int32_t main(int32_t argc, char** argv) {
	if (argc - 1 == 0) { fprintf(stderr, "usage: -o|(change )+"); return -1; }
	if (strcmp(argv[1], "-i") == 0) {
		if (init_fuel_tank() < 0) { fprintf(stderr, "ift: initialization failed\n"); return -1; }
	}
	else if (strcmp(argv[1], "-o") == 0) {
		if (print_fuel_tank() < 0) { fprintf(stderr, "pft: failed to print\n"); return -1; }
	}
	else {
    if (update_fuel_tank(argc, argv) < 0) { fprintf(stderr, "uft: failed to update the fuel tank"); return -1; }
  }
	return 0;
}

int32_t init_fuel_tank() {
	int32_t fd;
	float value;
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = sizeof(float);
	printf("Insert the amount of starting fuel for the fuel tank (must be a positive real number): ");
	while (scanf("%f", &value) != 1 && value < 0) {
    printf("You haven't introduced a positive real number, try again: ");
  }
	if ((fd = creat("fuel_tank.bin", 0600)) == -1) { perror("creat"); return -1; }
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("lock"); return -1; }
  if (ftruncate(fd, sizeof(float)) == -1) { perror("ftruncate"); return -1; }
	if (write(fd, &value, sizeof(float)) == -1) { perror("write"); return -1; }
  lock.l_type = F_UNLCK;
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("unlock"); return -1; }
	if (close(fd) == -1) { perror("close"); return -1; }
  return 0;
}
int32_t print_fuel_tank() {
	int32_t fd;
	float value;
  struct flock lock;
  lock.l_type = F_RDLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = 0;
	if ((fd = open("fuel_tank.bin", O_RDONLY, 0600)) == -1) { perror("open"); return -1; }
  if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("lock"); return -1; }
	if (read(fd, &value, sizeof(float)) == -1) { perror("read"); return -1; }
	if (close(fd) == -1) { perror("close"); return -1; }
	printf("The fuel tank has %f L of fuel.\n", value);
  return 0;
}
int32_t update_fuel_tank(int32_t n, char** nr) {
	float change, quantity;
	int32_t fd, i, option;
  struct flock lock;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_SET;
  lock.l_start = 0;
  lock.l_len = sizeof(float);
	if ((fd = open("fuel_tank.bin", O_RDWR), 0600) == -1) { perror("open"); return -1; }
  i = 1;
	while (i < n) {
		if (sscanf(nr[i], "%f", &change) != 1) {
      fprintf(stderr, "You've introduced a bad value as input(%s).\n", nr[i]);
      return -2;
    }
    if (fcntl(fd, F_SETLKW, &lock) == -1) { perror("lock"); return -1; }
		if (lseek(fd, 0, SEEK_SET) == -1) { perror("lseek"); return -1; }
		if (read(fd, &quantity, sizeof(float)) == -1) { perror("read"); return -1; }
    if (quantity + change < 0) {
      printf("extraction operation couldn't be done due to the fuel tank not having enough fuel. What would you ");
      printf("like to do?\n1. Perform a partial extraction, leaving the fuel tank empty.\n2. Skip this extraction.\n");
      printf("Your choice: ");
      if (scanf("%d", &option) != 1) { perror("scanf"); return -1; }
      //  this scanf call does not work for some reason
      printf("%d\n\n", option);
      if (option == 1) {
        printf("[PID: %d] The fuel tank used to have %f L of fuel. no fuel has been removed. The fuel tank now has \
0 L of fuel.\n", getpid(), quantity);
        quantity = 0;
      }
      else if (option == 2) {
        printf("[PID: %d] In compliance with your choice, the fuel tank has not been modified. It'll still have %f \
L of fuel.\n", getpid(), quantity);
      }
      else {
        fprintf(stderr, "Invalid option has been passed. All the other operations will be stopped.\n");
        return -2;
      }
    }
    else {
      printf("[PID: %d] The fuel tank used to have %f L of fuel. %f L have been %s. The fuel tank now has \
%f L of fuel.\n", getpid(), quantity, (change > 0 ? change : -change), (change > 0 ? "added" : "removed"), quantity + change);
      quantity = quantity + change;
    }
		if (lseek(fd, 0, SEEK_SET) == -1) { perror("lseek"); return -1; }
		if (write(fd, &quantity, sizeof(float)) == -1) { perror("write"); return -1; }
		lock.l_type = F_UNLCK;
    if (fcntl(fd, F_SETLK, &lock) == -1) { perror("unlock"); return -1; }
    lock.l_type = F_WRLCK;
    ++i;
	}
	if (close(fd) == -1) { perror("close"); return -1; }
	printf("[PID: %d] This process has stopped processing stuff.\n", getpid());
  return 0;
}
