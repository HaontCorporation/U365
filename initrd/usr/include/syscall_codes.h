#ifndef SYSCALL_CODES
#define SYSCALL_CODES

#define SYS_read      0
#define SYS_write     1
#define SYS_open      2
#define SYS_close     3
#define SYS_stat      4
// fstat 5
// lstat 6
// poll 7
#define SYS_lseek     8
#define SYS_mmap      9
// mprotect 10
#define SYS_munmap   11

#define SYS_exit     60
#define SYS_kill     62
#define SYS_uname    63

#define SYS_getdents 78
#define SYS_getcwd   79
#define SYS_chdir    80

#define SYS_creat    85
// Big 326-512 hole
#define SYS_execve  520

// Extended set
#define SYS_T_alloc   600
#define SYS_T_env_get 602
#define SYS_T_env_set 603
#define SYS_T_screen_apply 604

#endif
