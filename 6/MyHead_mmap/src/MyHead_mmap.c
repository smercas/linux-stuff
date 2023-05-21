#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    int opt, n = 10, c = 0;
    char* buf;
    int num_files = 0;
    struct stat sb;

    // Parse command-line options
    while ((opt = getopt(argc, argv, "n:c:")) != -1) {
        switch (opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'c':
                c = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s [-n num|-c num] file1 [file2 ...]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    /* Determinăm dimensiunea paginilor corespunzătoare arhitecturii hardware a calculatorului dvs. */
    long PageSize = sysconf(_SC_PAGE_SIZE);
    if (PageSize == -1)
    { perror("Error at sysconf"); return 6; }

    // Process each file specified in the command line
    for (int i = optind; i < argc; i++)
    {
        int fd = open(argv[i], O_RDONLY);
        if (fd == -1)
        { perror(argv[i]); continue; }

        if (fstat(fd, &sb) == -1)   /* obtain file size */
        { perror("Error at fstat"); continue; }

        num_files++;

        // Print the name of the file, if more than one file is specified
        if (num_files > 1)
        {
            printf("\n==> %s <==\n", argv[i]);
        }

        // Read and print the first n lines or c bytes from the file
        int num_lines = 0, num_bytes = 0;
        long offset = 0;
        while ((offset < sb.st_size) && ((n > 0 && num_lines <= n) || (c > 0 && num_bytes <= c))) {
            long length = PageSize;
            if(offset + length > sb.st_size) {
                length = sb.st_size - offset;
            }

            buf = mmap(NULL, length, PROT_READ, MAP_PRIVATE, fd, offset);
            if (buf == MAP_FAILED) {
                perror("Error at mmap (in)");
                break;
            } //while

            for (int j = 0; j < length; j++)
            {
                if (n > 0 && buf[j] == '\n')
                {
                    num_lines++;
                    if (num_lines >= n)
                    {
                        break; //for
                    }
                }

                if (c > 0)
                {
                    num_bytes++;
                    if (num_bytes > c)
                    {
                        break; //for
                    }
                }

                putchar(buf[j]);

            }

            if (-1 == munmap(buf, length))
            { perror("Error at munmap (in)"); break; } //while

            offset += length;

            if (n <= 0 || c <= 0 || num_bytes > c)
            {
                break;
            }
        }
        putchar('\n');
        close(fd);
    }

    // Check if no files were processed
    if (num_files == 0)
    {
        fprintf(stderr, "Error: You must specify at least one file to process\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}
