//EXT2 Filesystem support in kernel.
#include <arch/i686/ata.h>
#include <ext2.h>
#include <stdio.h>
#include <stdint.h>
#include <memory.h>
#include <math.h>
//is_valid_ext2: ext2 volume validation.
uint8_t is_valid_ext2(unsigned int ata_drive)
{
	struct ext2_superblock *superblock=(struct ext2_superblock *)malloc(1024);
	ata_read_bytes(ata_drive, (void*)superblock, 1024, 1024);
	if(superblock->ver_major<1)
		return 0; //Only EXT2 >= 1.* is supported now.
	if(superblock->signature!=0xef53)
		return 0; //EXT2 signature invalid.
	return 1;
}
//ext2_info:  Outputs ext2 volume information from superblock.
void ext2_info(unsigned int ata_drive)
{
	if(!is_valid_ext2(ata_drive))
	{
		fprintf(stderr, "[EXT2] Invalid ext2 filesystem on disk %d.", ata_drive);
		return;
	}
	struct ext2_superblock *superblock=malloc(sizeof(struct ext2_superblock));
	ata_read_bytes(ata_drive, (void*)superblock, sizeof(struct ext2_superblock), 1024);
	printf("EXT2 %u.%u volume with ATA number %u and volume label \"%s\" information:\n", superblock->ver_major, superblock->ver_minor, ata_drive, superblock->volume_label);
	printf("\tLast mounted on '%s'\n", superblock->last_mount_path);
	uint32_t block_size=1024 << superblock->block_size;
	uint32_t inode_size=1024 << superblock->inode_size;
	printf("\tFree space: %d/%d bytes (%d/%d blocks)\n", (superblock->blocks_count-superblock->unallocated_blocks_count) * block_size, superblock->blocks_count * block_size, superblock->blocks_count-superblock->unallocated_blocks_count, superblock->blocks_count);
	printf("\tFilesystem is %s\n", (superblock->fs_state==1) ? "clean" : "with errors");
	//Parse inode 2 and list root directory.
	printf("Block group descriptor table is on byte %d from start of the disk.\n", block_size * ((block_size == 1024) ? 2 : 1));
	uint32_t block_group_count = superblock->blocks_count / superblock->blocks_count_in_blk_group;
	struct ext2_block_group_descriptor *bgd=malloc(sizeof(struct ext2_block_group_descriptor) * block_group_count);
	ata_read_bytes(ata_drive, (void*)bgd, sizeof(struct ext2_block_group_descriptor) * block_group_count,
	  block_size * ((block_size == 1024) ? 2 : 1));
	printf("bgd->block_usage_address: %u\nbgd->inode_usage_address: %u\nbgd->inode_table_address: %u\nbgd->unallocated_blocks_count: %u\nbgd->unallocated_inodes_count: %u\nbgd->directories_in_group: %u\n", bgd->block_usage_address, bgd->inode_usage_address, bgd->inode_table_address, bgd->block_usage_address, bgd->unallocated_blocks_count, bgd->unallocated_inodes_count, bgd->directories_in_group);
	//get inode function:
	uint32_t inode = 2; //Root directory inode is defined to always be 2.
	uint32_t block_group = (inode - 1)  / superblock->inodes_count_in_blk_group;
	uint32_t inode_index = (inode - 1)  % superblock->inodes_count_in_blk_group;
	uint32_t block       = (inode_index * inode_size) / block_size;
	printf("\tInode: %d\n\tBlock group: %d\n\tInode index: %d\n\tInode block: %d\n\tUnknown value: %d", inode, block_group, inode_index, block, inode_index % (block_size / inode_size) * inode_size);
	bgd+=block_group;
	//uint32_t inodes_cnt  = superblock->inodes_count_in_blk_group - bgd->unallocated_inodes_count;
	printf("\tAllocating %u bytes,\n", sizeof(struct ext2_inode));
	printf("\tInode is on %u, offset in table: %u bytes, inode table is on block %u.\n", bgd->inode_table_address * block_size + block * block_size + inode_index % (block_size / inode_size) * inode_size, block * block_size, bgd->inode_table_address);
	struct ext2_inode *inode_struct = calloc(inode_size, 1);
	ata_read_bytes(ata_drive, (void*)inode_struct, inode_size, bgd->inode_table_address * block_size + block * block_size + inode_index % (block_size / inode_size) * inode_size);
	printf("\tInode is on block %u\n", bgd->inode_table_address + block);
	//inode_struct[inode_index].direct_block_pointers[0];
	printf("\tInode table is on block %u, block number of root directory is %u, block size is %u\n", bgd->inode_table_address, inode_struct->direct_block_pointers[0], block_size);
	struct ext2_directory_entry *dirent = calloc(sizeof(struct ext2_directory_entry), 0);
	//uint16_t name_length = dirent->name_length_low | (dirent->type_or_name_length_high << 8);
	ata_read_bytes(ata_drive, (void*)dirent, sizeof(struct ext2_directory_entry), inode_struct->direct_block_pointers[0] * block_size + sizeof(struct ext2_directory_entry));
	//dirent = realloc((void*)dirent, sizeof(struct ext2_directory_entry) + name_length);
	printf("Read inode from 0x%08X\n", inode_struct->direct_block_pointers[0]);
	//end
}