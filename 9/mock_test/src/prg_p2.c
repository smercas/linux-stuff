#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int32_t main() {
  int32_t read_code;
  int64_t number, i;
  while ((read_code = read(STDIN_FILENO, &number, sizeof(int64_t))) > 0) {
    if (number % 2 == 0) {
      number = number / 2;
      if (number % 3 == 0) {
        number = number / 3;
        if (number % 5 == 0) {
          number = number / 5;
        }
      }
    }
    if (write(STDOUT_FILENO, &number, sizeof(int64_t)) == -1) { fprintf(stderr, "number[%ld] write: ", i); perror(""); return -1; }
    ++i;
  }
  if (read_code == -1) { perror("read"); return -1; }
  return 0;
}
