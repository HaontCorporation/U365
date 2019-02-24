#ifndef EXT2_H
#define EXT2_H
#include <stdint.h>
enum ext2_file_type {
	FTYPE_UNKNOWN,
	FTYPE_REGULAR,
	FTYPE_DIRECTORY,
	FTYPE_CHARDEV,
	FTYPE_BLOCKDEV, //Like harddrives.
	FTYPE_FIFO,
	FTYPE_SOCKET,
	FTYPE_SYMLINK,
};
enum ext2_inode_type {
	ITYPE_FIFO = 0x1000,
	ITYPE_CHARDEV = 0x2000,
	ITYPE_DIRECTORY = 0x4000,
	ITYPE_BLOCKDEV = 0x6000, //Like harddrives.
	ITYPE_REGULAR = 0x8000,
	ITYPE_SYMLINK = 0xA000,
	ITYPE_SOCKET = 0xC000,
};
struct ext2_superblock
{
	uint32_t inodes_count;
	uint32_t blocks_count;
	uint32_t reserved_blocks_count;
	uint32_t unallocated_blocks_count;
	uint32_t unallocated_inodes_count;
	uint32_t superblock_block_number;
	uint32_t block_size;
	uint32_t fragment_size;
	uint32_t blocks_count_in_blk_group;
	uint32_t fragments_count_in_blk_group;
	uint32_t inodes_count_in_blk_group;
	uint32_t last_mount_time;
	uint32_t last_written_time;
	uint16_t times_mounted_after_check;
	uint16_t mounts_before_check;
	uint16_t signature;
	uint16_t fs_state;
	uint16_t action_on_error;
	uint16_t ver_minor;
	uint32_t last_check_time;
	uint32_t check_interval;
	uint32_t creator_id;
	uint32_t ver_major;
	uint16_t reserved_blocks_uid;
	uint16_t reserved_blocks_gid;
	uint32_t first_nonreserved_inode;
	uint16_t inode_size;
	uint16_t superblock_block_group;
	uint32_t optional_features;
	uint32_t required_features;
	uint32_t features_to_read_write;
	uint8_t  fs_id[16];
	char     volume_label[16];
	char     last_mount_path[64];
	uint32_t compression_algorithms_used;
	uint8_t blocks_to_preallocate_for_files;
	uint8_t blocks_to_preallocate_for_dirs;
	uint16_t unused;
	uint8_t  journal_id[16];
	uint32_t journal_inode;
	uint32_t journal_device;
	uint32_t orphan_inode_list_head;
} __attribute__((packed));
struct ext2_inode
{
	uint16_t type_and_perms;
	uint16_t owner_uid;
	uint32_t size_low;
	uint32_t access_time;
	uint32_t creation_time;
	uint32_t modified_time;
	uint32_t deletion_time;
	uint16_t owner_gid;
	uint16_t link_count; //These links are "pointers" to this inode.
	uint32_t sector_count;
	uint32_t flags;
	uint32_t os_specific;
	uint32_t direct_block_pointers[12]; //For medium-size files this is enough.
	uint32_t singly_indir_block_pointer;
	uint32_t double_indir_block_pointer;
	uint32_t triple_indir_block_pointer;
	uint32_t generation_number;
	uint32_t file_acl;
	uint32_t dir_acl_or_file_size_high;
	uint32_t fragment_address;
	uint8_t  os_specific_2[12];
} __attribute__((packed));
struct ext2_block_group_descriptor
{
	uint32_t block_usage_address;
	uint32_t inode_usage_address;
	uint32_t inode_table_address;
	uint16_t unallocated_blocks_count;
	uint16_t unallocated_inodes_count;
	uint16_t directories_in_group;
	uint16_t unused[7];
} __attribute__((packed));
struct ext2_directory_entry
{
	uint32_t inode;
	uint16_t size;
	uint8_t  name_length_low;
	uint8_t  type_or_name_length_high;
	//As you can see, structure misses name field. It's because C doesn't allow variable-length arrays in structures, because it will prevent compile-time structure size computing.
} __attribute__((packed));
void ext2_info(unsigned int);
#endif