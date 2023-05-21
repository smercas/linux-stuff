#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

int32_t main() {
  char arr[256];
  uint64_t i;
  int32_t pipe_1[2];
  int32_t pipe_2[2];
  int32_t myFifo;
  pid_t pid1, pid2;
  if (pipe(pipe_1) == -1) { perror("pipe"); return -1; }
  if (mkfifo("channel", 0600)  == -1 && errno != EEXIST) { perror("mkfifo"); return -1; }
  pid1 = fork();
  if (pid1 == -1) { perror("fork"); return -1; }
  else if (pid1 == 0) {
    //first child process:
    uint64_t j;
    //recieve the original char array from the parent process (through the "pipe_1" pipe)
    if (close(pipe_1[1]) == -1) { perror("close"); return -1; }
    if (read(pipe_1[0], arr, sizeof(arr)) != sizeof(arr) && errno != EEXIST) { perror("read"); return -1; }
    if (close(pipe_1[0]) == -1) { perror("close"); return -1; }
    //remove all the vowels
    j = 0;
    i = 0;
    while (i < strlen(arr)) {
      if (!strchr("aeiouAEIOU", arr[i])) {
        arr[j] = arr[i];
        ++j;
      }
      ++i;
    }
    arr[j] = '\0';
    //send the modified char array to the second child process (through the "channel" fifo file)
    if ((myFifo = open("channel", O_WRONLY)) == -1) { perror("open"); return -1; }
    if (write(myFifo, arr, sizeof(arr)) == -1) { perror("write"); return -1; }
    if (close(myFifo) == -1) { perror("close"); return -1; }
    return 0;
  } 
  else {
    if (pipe(pipe_2) == -1) { perror("pipe"); return -1; }
    pid2 = fork();
    if (pid2 == -1) { perror("fork"); return -1; }
    else if(pid2 == 0) {
      //second child process:
      //close the "pipe_1" pipe because i'm actually autistic
      if (close(pipe_1[0]) == -1) { perror("close"); return -1; }
      if (close(pipe_1[1]) == -1) { perror("close"); return -1; }
      //recieve a char array from the first process (through the "channel" fifo file)
      if ((myFifo = open("channel", O_RDONLY)) == -1) { perror("open"); return -1; }
      if (read(myFifo, arr, sizeof(arr)) == -1) { perror("read"); return -1; }
      if (close(myFifo) == -1) { perror("close"); return -1; }
      //turn all the lowercase letters to uppercase
      i = 0;
      while (i < strlen(arr)) {
        if (arr[i] >= 'a' && arr[i] <= 'z') {
          arr[i] = arr[i] - 32;
        }
        ++i;
      }
      //send the modified char array to the parent process (through the "pipe_2" pipe)
      if (close(pipe_2[0]) == -1) { perror("close"); return -1; }
      if (write(pipe_2[1], arr, sizeof(arr)) == -1) { perror("write"); return -1; }
      if (close(pipe_2[1]) == -1) { perror("close"); return -1; }
      return 0;
    } 
    else {
      //parent process:
      printf("Introduce some characters here:\n");
      //read a char array from the screen
      fgets(arr, sizeof(arr), stdin);
      i = 0;
      while(arr[i] != '\n') {
        ++i;
      }
      arr[i] = '\0';
      //send it to the first child process (through the "pipe_1" pipe)
      if (close(pipe_1[0]) == -1) { perror("close"); return -1; }
      if (write(pipe_1[1], arr, sizeof(arr)) == -1) { perror("write"); return -1; }
      if (close(pipe_1[1]) == -1) { perror("close"); return -1; }
      //recieve the modified char array from the second child process (through the "pipe_2" pipe)
      if (close(pipe_2[1]) == -1) { perror("close"); return -1; }
      if (read(pipe_2[0], arr, sizeof(arr)) == -1) { perror("read"); return -1; }
      if (close(pipe_2[0]) == -1) { perror("close"); return -1; }
      //print it
      printf("The result is:\n%s\n", arr);
      return 0;
    }
  }
}
