#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <stddef.h>
#include <stdint.h>

typedef uint32_t fs_node_id_t;
//typedef struct Filesystem Filesystem;
typedef struct DIR DIR;
typedef uint32_t ino_t;
typedef struct fs_interface fs_interface;
typedef struct iftab_entry iftab_entry;

#define S_IFMT   0170000 // BIT MASK

#define S_IFSOCK 0140000 //  socket
#define S_IFLNK  0120000 //  symbolic link
#define S_IFREG  0100000 //  regular file
#define S_IFBLK  0060000 //  block device
#define S_IFDIR  0040000 //  directory
#define S_IFCHR  0020000 //  character device
#define S_IFIFO  0010000 //  FIFO

#define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#define S_ISBLK(m) (((m) & S_IFMT) == S_IFBLK)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#define S_ISCHR(m) (((m) & S_IFMT) == S_IFCHR)

#include <vector.h>
#include <fio.h>
#include <strmap.h>
#include <mtab.h>
#include <proctab.h>

struct fs_interface
{
	const char* name;
	uint8_t (*get_inode)  (FILE*, fs_interface*, uint64_t, ino_t*); // Gets inode data
	uint8_t (*create_file)(FILE*, fs_interface*, const char*, mode_t);
	uint8_t (*delete_file)(FILE*, fs_interface*, const char*);
	uint8_t (*create_dir) (FILE*, fs_interface*, const char*, mode_t);
	uint8_t (*delete_dir) (FILE*, fs_interface*, const char*);

	int     (*open_file)  (FILE*, fs_interface*, const char*, int, mode_t, file_interface*);
	FILE*   (*fopen_file) (FILE*, fs_interface*, const char*, const char*);
	DIR*    (*opendir)    (FILE*, fs_interface*, const char*);
	int     (*stat)       (FILE*, fs_interface*, const char* path, struct stat* to);
};

// iftab - fs interface table
struct iftab_entry
{
	fs_interface* iface;
	iftab_entry* _next;
	iftab_entry* _prev;
};

struct iftab
{
	iftab_entry* _end;
};

extern struct iftab __iftab;

struct dirent
{
	// IU
	struct dirent* prev;
	struct dirent* next;
//	fs_node* node;
	// OU
	ino_t d_ino;
	char* d_name;
};

struct DIR
{
//	fs_node*       node;
	struct dirent* current;
	struct dirent* first;
};

struct stat
{
	dev_t     st_dev;         /* ID of device containing file */
	ino_t     st_ino;         /* inode number */
	mode_t    st_mode;        /* file type and mode */
/*	nlink_t*/   int st_nlink;       /* number of hard links */
//	uid_t     st_uid;         /* user ID of owner */
//	gid_t     st_gid;         /* group ID of owner */
	dev_t     st_rdev;        /* device ID (if special file) */
/*	off_t*/     int st_size;        /* total size, in bytes */
//	blksize_t st_blksize;     /* blocksize for filesystem I/O */
//	blkcnt_t  st_blocks;      /* number of 512B blocks allocated */
};

typedef enum
{
	LS_COLOR = 1,
	LS_HIDDEN = 2,
	LS_POINTS = 4,
	LS_SYMBOLS = 8,
	LS_BACKUPS = 16,
	LS_NEWLINE = 32,
} ls_flags_t;

void iftab_init();
void iftab_add(fs_interface* iface);

int   fs_create_file(FILE*, fs_interface*, const char*, mode_t);
int   fs_delete_file(FILE*, fs_interface*, const char*);
int   fs_create_dir (FILE*, fs_interface*, const char*, mode_t);
int   fs_delete_dir (FILE*, fs_interface*, const char*);
int   fs_is_file    (FILE*, fs_interface*, const char*);
int   fs_is_dir     (FILE*, fs_interface*, const char*);
int   fs_open_file  (FILE*, fs_interface*, const char*, int, mode_t, file_interface*);

FILE* fs_fopen_file (FILE*, fs_interface*, const char*, const char*);
DIR*  fs_opendir    (FILE*, fs_interface*, const char*);
int   fs_stat       (FILE*, fs_interface*, const char*, struct stat*);
int   stat          (                      const char*, struct stat*);

DIR*           opendir(const char*);
struct dirent* readdir(DIR*);
void           closedir(DIR*);
int fs_list_files(const char* path, ls_flags_t flags);
char* expand_path(const char* src);

#endif // FILESYSTEM_H
