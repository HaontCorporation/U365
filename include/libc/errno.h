#ifndef ERRNO_H
#define ERRNO_H

extern int errno;
enum {ENOENT=1, ENXIO, EROFS, ENOMEM, EACCES, EPERM, ENODEV};
const char* strerror(int);
void error(int, int, const char*, ...);

#endif //ERRNO_H
