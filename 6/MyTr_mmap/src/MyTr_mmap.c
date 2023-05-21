#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>

int main (int argc, char *argv []) {
    int input_fd, output_fd;
    struct stat sb;
    char* buffer_in;
    char* buffer_out;

    if (argc != 5) {
        printf("Usage: MyTr_mmap file-src file-dest c1 c2 \n");  return 1;
    }
    input_fd = open(argv[1], O_RDONLY);
    if (input_fd == -1) {
        perror(argv[1]);  return 2;
    }

    if (fstat(input_fd, &sb) == -1)   /* To obtain file size */
    { perror("Error at fstat"); return 5; }

    output_fd = open(argv[2], O_RDWR|O_CREAT|O_TRUNC, 0600);
    if (output_fd == -1) {
        perror(argv[2]);  return 3;
    }
    ftruncate(output_fd, sb.st_size); /* prealocate destination file */

    /* Determinăm dimensiunea paginilor corespunzătoare arhitecturii hardware a calculatorului dvs. */
    long PageSize = sysconf(_SC_PAGE_SIZE);
    if (PageSize == -1) {
      perror("Error at sysconf");
      return 6;
    }

    // fprintf(stderr, "Notification: the page size on your hardware system is: %ld bytes.\n", PageSize);

    char c1 = *argv[3];
    /*char c1 = argv[3][0];*/
    char c2 = *argv[4];
    /* Process the input file a record at atime. */
    off_t offset = 0;
    while(offset < sb.st_size)
    {
        long length = PageSize;
        if(offset + length > sb.st_size)
        {
            length = sb.st_size - offset;
        }

        /* Apelul de mapare a porțiunii de fișier specificate, în memorie. */
        buffer_in = mmap(NULL,           // Se va crea o mapare începând de la o adresă page-aligned aleasă de kernel (și returnată în map_addr)
                         length,         // Lungimea ne-aliniată (i.e. ne-ajustată page-aligned) a mapării (doar corectată, ca mai sus)
                         PROT_READ,      // Tipul de protecție a mapării: paginile mapării vor permite doar accese în citire
                         MAP_PRIVATE,    // Maparea este privată (i.e., nu este partajată cu alte procese)
                         input_fd,       // Descriptorul de fișier, din care se citesc octeții corespunzători porțiunii mapate în memorie
                         offset          // Offset-ul (deplasamentul) ne-aliniat (i.e. ne-ajustat page-aligned), de la care începe porțiunea de fișier mapată
                   );                    // în memorie, i.e. porțiunea din fișier din care se citesc cei actual_length octeți, în maparea din memorie
        if (buffer_in == MAP_FAILED)
        { perror("Error at mmap (in)"); return 7; }

        buffer_out = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, output_fd, offset);
        if (buffer_out == MAP_FAILED) {
          perror("Error at mmap (out)");
          return 7;
        }

        for(int i = 0; i < length; i++) {
            if (buffer_in[i] == c1)
                buffer_out[i] = c2;
            else
                buffer_out[i] = buffer_in[i];
        }
        if (-1 == msync(buffer_out, length, MS_SYNC)) {
          perror("Error encountered while sybnchronizing the output file");
          return 11;
        }
        // "Ștergem/distrugem" maparea creată anterior.
        if (-1 == munmap(buffer_in, length)) {
          perror("Error at munmap (in)");
          return 10;
        }
        if (-1 == munmap(buffer_out, length)) {
          perror("Error at munmap (out)");
          return 10;
        }

        offset += length;
    }

    close(input_fd);
    close(output_fd);
    return 0;
}
