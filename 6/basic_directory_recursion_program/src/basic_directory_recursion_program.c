#include <stdio.h>
// #include <stdlib.h>      // has malloc, free, atoi, atof in it
#include <string.h>
// #include <sys/types.h>   // useful if you need to use size_t
#include <sys/stat.h>
#include <sys/sysmacros.h>  // for major and minor in this case
#include <time.h>           // for fancy time strings when using atime / ctime / mtime
#include <unistd.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <linux/limits.h>  // including this will get rid ot errors that are not really errors
                              // (like PATH_MAX being undefined), although it might be useful in some cases
#include <stdint.h>
#include "print_args.h"
#include "file_work/print_time.h"
#include "file_work/is_directory.h"
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
/*
stat contains:
  dev_t     st_dev;         // describes the device on which this file resides
    to ever use this(god forbid):
      don't forget to include #include <sys/sysmacros.h>

      stat(pathname, &sb);
      a = major(st.st_dev);     // a will have the major components of the device id
      b = minor(st.st_dev);     // b will have the minor components of the device id
  ino_t     st_ino;         // contains the file's inode number
  mode_t    st_mode;        // contains the file type and mode
    to test the file type:
      masks:
        __S_IFMT     0170000   // bit mask for the file type bit field

        __S_IFSOCK   0140000   // socket
        __S_IFLNK    0120000   // symbolic link
        __S_IFREG    0100000   // regular file
        __S_IFBLK    0060000   // block device
        __S_IFDIR    0040000   // directory
        __S_IFCHR    0020000   // character device
        __S_IFIFO    0010000   // FIFO

      macros:
        S_ISSOCK(m) // socket
        S_ISLNK(m)  // symbolic link
        S_ISREG(m)  // is it a regular file
        S_ISBLK(m)  // block device
        S_ISDIR(m)  // directory
        S_ISCHR(m)  // character device
        S_ISFIFO(m) // FIFO

      to test for a regular file you can use:
        stat(pathname, &sb);

        if ((sb.st_mode & S_IFMT) == S_IFREG) {
          // Handle regular file
        }
        OR
        if (S_ISREG(sb.st_mode)) {
          // Handle regular file
        }
    to test the permissions:
      S_ISUID     04000   // set-user-ID bit
      S_ISGID     02000   // set-group-ID bit
      S_ISVTX     01000   // sticky bit

      S_IRWXU     00700   // owner has read, write, and execute permission
      S_IRUSR     00400   // owner has read permission
      S_IWUSR     00200   // owner has write permission
      S_IXUSR     00100   // owner has execute permission

      S_IRWXG     00070   // group has read, write, and execute permission
      S_IRGRP     00040   // group has read permission
      S_IWGRP     00020   // group has write permission
      S_IXGRP     00010   // group has execute permission

      S_IRWXO     00007   // others have read, write, and execute permission
      S_IROTH     00004   // others have read permission
      S_IWOTH     00002   // others have write permission
      S_IXOTH     00001   // others have execute permission

      to test for user read permission you can use:
        stat(pathname, &sb)
        if(sb.st_node & S_IRUSR) {
          // read that shit
          // or do whatev
        }
  nlink_t   st_nlink;       // contains the number of hard links to the file
  uid_t     st_uid;         // contains the user ID of the owner of the file
  gid_t     st_gid;         // contains the ID of the group owner of the file
  dev_t     st_rdev;        // This field describes the device that this file (inode) represents (if special file)
  off_t     st_size;        // gives the size of the file in bytes.
                            // if the file is a symbolic link:
                            //    gives the length of the pathname it contains, without a terminating null byte
  blksize_t st_blksize;     // gives the "preferred" block size for efficient filesystem I/O
  blkcnt_t  st_blocks;      // indicates the number of blocks allocated to the file, in 512-byte units

  struct timespec st_atim;  // time of last access
  struct timespec st_mtim;  // time of last modification
  struct timespec st_ctim;  // time of last status change
*/
  struct passwd* dpasswd;
  struct group* dgroup;
  char permissions[10] = "---------";
  char time_string[100] = "";
  int32_t isDirectory = 0;

  if (stat(path, &dstat) == -1) psi_s_err

  printf("File: %s\n", path);

  printf("Device: %u / %u\n", major(dstat.st_dev), minor(dstat.st_dev));

  printf("Inode: %lu\n", dstat.st_ino);

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

  printf("Number of hard links: %lu\n", dstat.st_nlink);

  printf("Uid: %u\n", dstat.st_uid);
  
  dpasswd=getpwuid(dstat.st_uid);
  if (dpasswd != NULL) {
    printf("Username: %s\n", dpasswd->pw_name);
  }

  printf("Gid: %u\n", dstat.st_gid);
  dgroup=getgrgid(dstat.st_gid);
  if (dgroup != NULL) {
    printf("Group Name: %s\n", dgroup->gr_name);
  }

  printf("Size: %ld\n", dstat.st_size);

  printf("IO block: %ld\n", dstat.st_blksize);

  printf("Size allocated on the disk(in 512b blocks): %lu\n", dstat.st_blocks);

  print_time(dstat.st_atime, time_string, sizeof(time_string), "access");

  print_time(dstat.st_mtime, time_string, sizeof(time_string), "modification");

  print_time(dstat.st_ctime, time_string, sizeof(time_string), "status change");

  return isDirectory;
}

//  -1 - open
//  -2 - close
//  -3 - rec
int32_t rec(char *path) {
  printf("%s\n", path);
  //if (print_stat_info(path) != 1) {
  if (is_directory(path) != 1) {
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
