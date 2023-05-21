#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>

int32_t main(int32_t argc, char** argv) {
    int32_t   fd_read, fd_write, fd_readwrite, code_read;
    char* fifo_name;
    char message[4096];
    if (argc - 1 != 2) { fprintf(stderr,"usage: fifo_file_name info\n"); return -1; }
    fifo_name = argv[1];
    strcpy(message, argv[2]);
    if (mkfifo(fifo_name, 0600) == -1 && errno != EEXIST) { perror("mkfifo"); return  -1; }
    if ((fd_read = open(fifo_name, O_RDONLY | O_NONBLOCK )) == -1) { perror("open (read)"); return -1;}
    if ((fd_write = open(fifo_name, O_WRONLY)) == -1) { perror("open (write)"); return -1; }
    //
    if (write(fd_write, message, strlen(message)) == -1) { perror("write"); return -1; }
    if ((code_read = read(fd_read, message, strlen(message))) == -1) { perror("read"); return -1; }
    printf("%s\n", message);
    //
    if ((fd_readwrite = open(fifo_name, O_RDWR | O_NONBLOCK)) == -1) { perror("read-write open"); return -1; }
    if (close(fd_read) == -1) { perror("close"); return -1; }
    if (close(fd_write) == -1) { perror("close"); return -1; }
    //
    
    //
    return 0;
}
