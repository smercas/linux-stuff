#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc, char** argv) {
  int fd, read_code;
  unsigned char ch;
  struct flock lock;

  if (argc - 1 != 1) return -1;
  if ((fd = open("fis.dat", O_RDWR)) == -1) return -1;
  lock.l_type = F_WRLCK;
  lock.l_whence = SEEK_CUR;
  lock.l_start = -1;
  lock.l_len = 1;
  while ((read_code = read(fd, &ch, 1)) > 0) {
    if(ch == '#') {
      if (fcntl(fd, F_SETLKW, &lock) == -1) return -1;
      if (lseek(fd, -1, SEEK_CUR) == -1) return -1;
      if (read(fd, &ch, 1) == -1) return -1;
      if (ch == '#') {
        if (lseek(fd, -1, SEEK_CUR) == -1) return -1;
        if (write(fd, argv[1], 1) == -1) return -1;
        return 0;
      }
      else {
        lock.l_type = F_UNLCK;
        if (fcntl(fd, F_SETLK, &lock) == -1) return -1;
        lock.l_type = F_WRLCK;
      }
    }
  }
  if (read_code == -1) return -1;
  close(fd);
  printf("No '#' character found.\n");
  return 1;
}
