#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char* argv[])
{
    char* input=argv[1];
    char* output=argv[2];
    int f1=open(input,O_RDONLY);
    if (f1 == -1) {
      perror("Error encountered while opening a file");
      return -1;
    }
    struct stat s;
    if (fstat(f1,&s) == -1) {
      perror("Error encountered while recieving info about the input file");
      return -2;
    }

    ftruncate(f1, s.st_size);

    long pageSize = sysconf(_SC_PAGE_SIZE);
    if (pageSize == -1) {
      perror("Error encountered while recieving system info(more specifically, the size of a page)");
      return -3;
    }

    off_t offset = 0;

    long length = pageSize;
    if (offset + length > s.st_size) {
      length = s.st_size - offset;
    }
    char* map=mmap(NULL,length,PROT_READ,MAP_PRIVATE,f1,offset);
    int f2=open(output,O_WRONLY);
    if (f2 == -1) {
      perror("Error encountered while opening a file");
      return -1;
    }
    ftruncate(f2, s.st_size);
    char* mapp=mmap(NULL,length, PROT_READ | PROT_WRITE,MAP_SHARED,f2,offset);
    //mapp[0]=(char*)map[0];
printf("X\n\n");
    mapp[0] = map[0];
   // mapp[2]=map[2];
    msync(mapp,s.st_size,MS_SYNC);
    munmap(map,s.st_size);
    munmap(mapp,s.st_size);
    close(f1);
    close(f2);
    return 0;
}