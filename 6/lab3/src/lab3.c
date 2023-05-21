#include <stdio.h>
// #include <stdlib.h>      // has malloc, free, atoi, atof in it
#include <string.h>
// #include <sys/types.h>   // useful if you need to use size_t
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <linux/limits.h>  //including this will get rid ot errors that are not really errors
                              //(like PATH_MAX being undefined), although it might be useful in some cases
#include <stdint.h>
#include "print_args.h"
#include "file_work/recieve_input_directory.h"

char directory_name[PATH_MAX];

#define psi_s_err { fprintf(stderr, "%s: Error encountered while recieving information about a directory (%s).\n", __func__, path); return -1; }

#define r_o_err { fprintf(stderr, "%s: Error encountered while opening a directory (%s).\n", __func__, path); return -1; }
#define r_c_err { fprintf(stderr, "%s: Error encountered while closing a directory (%s).\n", __func__, path); return -2; }
#define r_r_err { fprintf(stderr, "%s: Error encountered while accessing a directory (%s).\n", __func__, newpath); return -3; }

#define m_bac_err { fprintf(stderr, "%s: the number of arguments that have been passed is %d when it should be %d.\n", __func__, argc - 1, 1); return -1; }
#define m_rid_err { fprintf(stderr, "%s: Error encountered while recieving the input directory.\n", __func__); return -2; }
#define m_r_err { fprintf(stderr, "%s: Error encountered while accessing a directory (%s).\n", __func__, directory_name); return -3; }

// using stat to get information about a file
//  -1 - stat
int32_t print_stat_info(char* path) {
  struct stat dstat;
  struct passwd* dpasswd;
  struct group* dgroup;
  char permissions[10]="---------";
  int32_t isDirectory=0;

  if (stat(path, &dstat) == -1) psi_s_err

  printf("Name: %s\n", path);

  printf("Type: ");
  switch (dstat.st_mode & __S_IFMT) {
    case __S_IFBLK:   printf("block device\n");             break;
    case __S_IFCHR:   printf("character device\n");         break;
    case __S_IFDIR:   printf("directory\n"); isDirectory=1; break;
    case __S_IFIFO:   printf("FIFO/pipe\n");                break;
    case __S_IFLNK:   printf("symlink\n");                  break;
    case __S_IFREG:   printf("regular file\n");             break;
    case __S_IFSOCK:  printf("socket\n");                   break;
    default:          printf("unknown?\n");                 break;
  }

  printf("Number of hard links: %lu\n", dstat.st_nlink);

  printf("Size: %lu\n", dstat.st_size);

  printf("Size allocated on the disk(in 512b blocks): %lu\n", dstat.st_blocks);

  printf("Permissions, in octal form: %o\n", dstat.st_mode & 0777);

  if(S_IRUSR & dstat.st_mode) permissions[0]='r';
  if(S_IWUSR & dstat.st_mode) permissions[1]='w';
  if(S_IXUSR & dstat.st_mode) permissions[2]='x';
  if(S_IRGRP & dstat.st_mode) permissions[3]='r';
  if(S_IWGRP & dstat.st_mode) permissions[4]='w';
  if(S_IXGRP & dstat.st_mode) permissions[5]='x';
  if(S_IROTH & dstat.st_mode) permissions[6]='r';
  if(S_IWOTH & dstat.st_mode) permissions[7]='w';
  if(S_IXOTH & dstat.st_mode) permissions[8]='x';

  printf("Permissions, in a more readable form: %s\n", permissions);

  printf("Uid: %u\n", dstat.st_uid);
  dpasswd=getpwuid(dstat.st_uid);
  if (dpasswd != NULL) {
    printf("Username: %s\n", dpasswd->pw_name);
  }

  printf("Gid: %u\n", dstat.st_gid);
  dgroup=getgrgid(dstat.st_gid);
  if (dgroup != NULL) {
    printf("Group Name: %s\n\n", dgroup->gr_name);
  }
  return isDirectory;
}

//  -1 - open
//  -2 - close
//  -3 - rec
int32_t rec(char *path) {
  if (print_stat_info(path) != 1) {
    // path is not a directory
  }
  else {
    // path is a directory
    DIR* d;
    struct dirent* de;
    char newpath[PATH_MAX];

    
    if ((d = opendir(path)) == NULL) r_o_err

    while ((de = readdir(d)) != NULL) {      
      if (strcmp(de->d_name, ".") && strcmp(de->d_name, "..")) {
        strcpy(newpath, path);
        strcat(newpath, "/");
        strcat(newpath, de->d_name);
        if (rec(newpath) < 0) r_r_err
      }
    } 
    
    if (closedir(d) == -1) r_c_err
  }
  return 0;
}

//  -1 - bad argument count
//  -2 - recieve_input_directory
//  -3 - rec
int32_t main(int32_t argc, char** argv) {
//  print_args(argc, argv);

  if (argc - 1 != 1) m_bac_err

  if (recieve_input_directory(argc, argv, directory_name, 1) < 0) m_rid_err

  if (rec(directory_name) < 0) m_r_err

  return 0;
}

#undef psi_m_err

#undef r_o_err
#undef r_c_err
#undef r_r_err

#undef m_bac_err
#undef m_rid_err
#undef m_r_err
