#ifndef TARFS_H
#define TARFS_H

#include <stdint.h>
#include <stddef.h>

typedef struct tarfs_data tarfs_data;

#include "fs.h"
#include "vector.h"
#include "tar.h"

struct tarfs_data
{
	const void* raw;
	size_t size; // in bytes
	size_t num;  // num of inodes
	vector* v;   // fast table of pointers for each file (tarfs_node)
};

void init_rootfs();

#endif // TARFS_H
