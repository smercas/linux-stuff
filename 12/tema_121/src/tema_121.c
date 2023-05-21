#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

int32_t main() {
  pid_t pid1, pid2, pid3;
  int32_t pipe1[2], pipe2[2], pipe3[2];
  if (pipe(pipe1) == -1) { perror("pipe"); return -1; }
  if (pipe(pipe2) == -1) { perror("pipe"); return -1; }
  if (pipe(pipe3) == -1) { perror("pipe"); return -1; }
  pid1 = fork();
  if (pid1 == -1) { perror("fork"); return -1; }
  else if (pid1 == 0) {
    if (close(pipe1[0]) == -1) { perror("close"); return -1; }
    if (close(pipe2[0]) == -1) { perror("close"); return -1; }
    if (close(pipe3[0]) == -1) { perror("close"); return -1; }
    if (close(pipe2[1]) == -1) { perror("close"); return -1; }
    if (close(pipe3[1]) == -1) { perror("close"); return -1; }
    if (dup2(pipe1[1], 1) == -1) { perror("dup2"); return -1; }
    if (close(pipe1[1]) == -1) { perror("close"); return -1; }
    execlp("grep", "grep", "sudo:", "/etc/group", NULL);  //_ -> 11
  }
  else {
    pid2 = fork();
    if (pid2 == -1) { perror("fork"); return -1; }
    else if (pid2 == 0) {
      if (close(pipe2[0]) == -1) { perror("close"); return -1; }
      if (close(pipe3[0]) == -1) { perror("close"); return -1; }
      if (close(pipe1[1]) == -1) { perror("close"); return -1; }
      if (close(pipe3[1]) == -1) { perror("close"); return -1; }
      if (dup2(pipe1[0], 0) == -1) { perror("dup2"); return -1; }
      if (close(pipe1[0]) == -1) { perror("close"); return -1; }
      if (dup2(pipe2[1], 1) == -1) { perror("dup2"); return -1; }
      if (close(pipe2[1]) == -1) { perror("close"); return -1; }
      execlp("cut", "cut", "-f4", "-d:", NULL); //10 -> 21
    }
    else {
      pid3 = fork();
      if(pid3 == -1) { perror("fork"); return -1; }
      else if(pid3 == 0) {
        if (close(pipe1[0]) == -1) { perror("close"); return -1; }
        if (close(pipe3[0]) == -1) { perror("close"); return -1; }
        if (close(pipe1[1]) == -1) { perror("close"); return -1; }
        if (close(pipe2[1]) == -1) { perror("close"); return -1; }
        if (dup2(pipe2[0], 0) == -1) { perror("dup2"); return -1; }
        if (close(pipe2[0]) == -1) { perror("close"); return -1; }
        if (dup2(pipe3[1], 1) == -1) { perror("dup2"); return -1; }
        if (close(pipe3[1]) == -1) { perror("close"); return -1; }
        execlp("tr", "tr", ",", "\n", NULL);  //20 -> 31
      }
      else {
        if (close(pipe1[0]) == -1) { perror("close"); return -1; }
        if (close(pipe2[0]) == -1) { perror("close"); return -1; }
        if (close(pipe1[1]) == -1) { perror("close"); return -1; }
        if (close(pipe2[1]) == -1) { perror("close"); return -1; }
        if (close(pipe3[1]) == -1) { perror("close"); return -1; }
        if (dup2(pipe3[0], 0) == -1) { perror("dup2"); return -1; }
        if (close(pipe3[0]) == -1) { perror("close"); return -1; }
        execlp("wc", "wc", "-l", NULL); //30 -> _
      }
    }
    return 0;
  }
}
