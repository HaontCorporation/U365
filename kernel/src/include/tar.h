#ifndef TAR_H
#define TAR_H

//First some TAR file structures
typedef struct tar_hdr tar_hdr_t;
struct tar_hdr
{
	char name[100];               /*   0 */
	char mode[8];                 /* 100 */
	char uid[8];                  /* 108 */
	char gid[8];                  /* 116 */
	char size[12];                /* 124 */
	char mtime[12];               /* 136 */
	char chksum[8];               /* 148 */
	char typeflag;                /* 156 */
	char linkname[100];           /* 157 */
	char magic[6];                /* 257 */
	char version[2];              /* 263 */
	char uname[32];               /* 265 */
	char gname[32];               /* 297 */
	char devmajor[8];             /* 329 */
	char devminor[8];             /* 337 */
	char prefix[155];             /* 345 */
								/* 500 */
	char unused[12]; //Unused bytes
};
unsigned int tar_getsize  (const char*);
unsigned int tar_gethdrcnt(const void*);

#define TAR_TYPE_DIR ('5')
#define TAR_TYPE_REG ('0')

#endif //TAR_H