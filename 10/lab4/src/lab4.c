#define _POSIX_SOURCE

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/time.h>
#include <sys/resource.h>

int main(void) {

//  For more info, visit the following man pages:
//  man 3 sysconf
//  man 2 getrlimit

  struct rlimit limits;

  //       struct rlimit {
  //           rlim_t rlim_cur;  /* Soft limit */
  //           rlim_t rlim_max;  /* Hard limit (ceiling for rlim_cur) */
  //       };
  // Notes: The glibc getrlimit() and setrlimit() wrapper functions use a 64-bit rlim_t data type, even on 32-bit platforms. 


//  limits referring to users and groups

  long UsernameMax = sysconf(_SC_LOGIN_NAME_MAX);
  if (UsernameMax == -1) { perror("sysconf"); return -1; }
  printf("\nThe maximum length of a login name, including the terminating null byte, is: %ld caracters.\n", UsernameMax);
  printf("   Note: the POSIX standard defines the minimum value for this limit as: %d.\n", _POSIX_LOGIN_NAME_MAX);

  long NGroupsMax = sysconf(_SC_NGROUPS_MAX);
  if (NGroupsMax == -1) { perror("sysconf"); return -1; }
  printf("\nThe maximum number of supplementary group IDs is: %ld groups.\n", NGroupsMax);

//  Limits referring to the hardware configuration of your system

  long NProcConf = sysconf(_SC_NPROCESSORS_CONF); //These values also exist, but may not be standard.
  if (NProcConf == -1) { perror("sysconf"); return -1; }
  long NProcAvail = sysconf(_SC_NPROCESSORS_ONLN); //These values also exist, but may not be standard.
  if (NProcAvail == -1) { perror("sysconf"); return -1; }
  printf("\nThe number of processors configured / currently online (available) are: %ld, respectively %ld,  processors.\n", NProcConf, NProcAvail);
  //  See also: get_nprocs_conf(3)

//  Limits referring to the sdministration of memory

  long PhysPages = sysconf(_SC_PHYS_PAGES); //These values also exist, but may not be standard.
  if (PhysPages == -1) { perror("sysconf"); return -1; }
  printf("\nThe number of pages of physical memory is: %ld pages.\n", PhysPages);

  long AvailPhysPages = sysconf(_SC_AVPHYS_PAGES); //These values also exist, but may not be standard.
  if (AvailPhysPages == -1) { perror("sysconf"); return -1; }
  printf("\nThe number of currently available pages of physical memory is: %ld pages.\n", AvailPhysPages);

//  Page size

  long PageSize = sysconf(_SC_PAGE_SIZE);
  if (PageSize == -1) { perror("sysconf"); return -1; }
  printf("\nThe page size on your hardware system is: %ld bytes.\n", PageSize);

  if (getrlimit(RLIMIT_AS, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_AS (i.e., the maximum size of the process's virtual memory (address space)), "\
          "configured for your system, are: %ld, respectively %ld, bytes.\n", limits.rlim_cur, limits.rlim_max);
  //  if the returned value is -1 (or RLIM_INFINITY), that means that this resource is "unlimited"
  //  to set a limit, use: setrlimit() for RLIMIT_AS

  if (getrlimit(RLIMIT_DATA, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_DATA (i.e., the maximum size of the process's data segment (initialized data, uninitialized data, and heap)), "\
          "configured for your system, are: %ld, respectively %ld, bytes.\n", limits.rlim_cur, limits.rlim_max);
  //  if the returned value is -1 (or RLIM_INFINITY), that means that this resource is "unlimited"
  //  to set a limit, use: setrlimit() for RLIMIT_DATA

  if (getrlimit(RLIMIT_STACK, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_STACK (i.e., the maximum size of the process stack, in bytes), "\
          "configured for your system, are: %ld, respectively %ld, bytes.\n", limits.rlim_cur, limits.rlim_max);
  //  if the returned value is -1 (or RLIM_INFINITY), that means that this resource is "unlimited"
  //  to set a limit, use: setrlimit() for RLIMIT_STACK

  if (getrlimit(RLIMIT_MEMLOCK, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_MEMLOCK (i.e., the maximum number of bytes of memory that may be locked into RAM), "\
          "configured for your system, are: %ld, respectively %ld, bytes.\n", limits.rlim_cur, limits.rlim_max);

//  Limits referring to program execution handling

  //  Max simultaneous active processes per user ID, Must not be less then _POSIX_CHILD_MAX(25).
  long ChildMax = sysconf(_SC_CHILD_MAX);
  if (ChildMax == -1) { perror("sysconf"); return -1; }
  printf("\nThe maximum number of simultaneous processes per user ID, configured for your system, is: %ld processes/user.\n", ChildMax);
  printf("   Note: the POSIX standard defines the minimum value for this limit as: %d.\n", _POSIX_CHILD_MAX);
  //  WARNING: beware of infinite recursion while using fork()
  //  to reduce the impact of said case, lower the limit of _SC_CHILD_MAX using setrlimit() for RLIMIT_NPROC

  if (getrlimit(RLIMIT_NPROC, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_NPROC (i.e., maximum number of simultaneous processes per user ID), "\
          "configured for your system, are: %ld, respectively %ld, processes/user.\n", limits.rlim_cur, limits.rlim_max);

  //  Max execution time on the CPU of a process(can be unlimited)
  if (getrlimit(RLIMIT_CPU, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_CPU (i.e., the limit, in seconds, on the amount of CPU time that the process can consume), "\
          "configured for your system, are: %ld, respectively %ld, seconds.\n", limits.rlim_cur, limits.rlim_max);
  //  if the returned value is -1 (or RLIM_INFINITY), that means that this resource is "unlimited"
  //  to set a limit, use: setrlimit() for RLIMIT_CPU

//  Limits referring to managing files that have been created/opened by programs

  long OpenFilesMax = sysconf(_SC_OPEN_MAX);
  //  The maximum number of files that a process can have open at any time.  Must not be less than _POSIX_OPEN_MAX (20).
  if (OpenFilesMax == -1) { perror("sysconf"); return -1; }
  printf("\nThe maximum number of simultaneous open files per process, configured for your system, is: %ld open files/process.\n", OpenFilesMax);
  printf("   Note: the POSIX standard defines the minimum value for this limit as: %d.\n", _POSIX_OPEN_MAX);

  if (getrlimit(RLIMIT_NOFILE, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_NOFILE (i.e., this specifies a value one greater than the maximum file descriptor number that can be opened by this process), "\
          "configured for your system, are: %ld, respectively %ld.\n", limits.rlim_cur, limits.rlim_max);

  if (getrlimit(RLIMIT_FSIZE, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_FSIZE (i.e., the maximum size in bytes for any file that the process may create), "\
          "configured for your system, are: %ld, respectively %ld, seconds.\n", limits.rlim_cur, limits.rlim_max);
  //  if the returned value is -1 (or RLIM_INFINITY), that means that this resource is "unlimited"
  //  to set a limit, use: setrlimit() for RLIMIT_FSIZE

  if (getrlimit(RLIMIT_LOCKS, &limits) == -1) { perror("getrimit"); return -1; }
  printf("\nThe soft/hard limits for RLIMIT_LOCKS (i.e., the limit on the combined number of flock(2) locks and fcntl(2) leases that this "\
          "process may establish), are: %ld, respectively %ld, seconds.\n", limits.rlim_cur, limits.rlim_max);
  //  if the returned value is -1 (or RLIM_INFINITY), that means that this resource is "unlimited"
  //  to set a limit, use: setrlimit() for RLIMIT_LOCKS

//  Other limits regarding system resources

  long clock_ticks = sysconf(_SC_CLK_TCK);
  //  The number of clock ticks per second.  The corresponding variable is obsolete.  It was of course called CLK_TCK.
  //  (Note: the macro CLOCKS_PER_SEC does not give information: it must equal 1000000.)

  if (clock_ticks == -1) { perror("sysconf"); return -1; }
  printf("\nNotification: The number of clock ticks per second (aka kernel constant USER_HZ), configured for your system's kernel, "\
          "is: %ld clock ticks/second.\n", clock_ticks);

  return 0;
}
