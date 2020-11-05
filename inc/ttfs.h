#ifndef TTFS_H
#define TTFS_H

#include <stdint.h>

#define TTFS_MAGIC "TTFS"
#define FILE_NAME_MAX_LENGTH 22
#define MAX_FILE_BLOCKS 9
#define BLOCK_SIZE 512

typedef struct __attribute__((packed)) ttfs_super_block {
	char ttfs_magic[4];
	uint32_t inode_count;
	uint32_t block_count;
	uint32_t offset_to_inode_map;
	uint32_t offset_to_block_map;
	uint32_t offset_to_inode_table;
	uint32_t offset_to_data_blocks;
	uint32_t unused;
} ttfs_super_block;

typedef struct __attribute__((packed)) ttfs_inode {
	uint8_t inode_used:1;
	uint8_t is_directory:1;
	uint8_t unused:6;
	uint8_t block_count;
	char file_name[FILE_NAME_MAX_LENGTH];
	uint32_t file_size;
	uint32_t blocks[MAX_FILE_BLOCKS];
} ttfs_inode;

typedef struct __attribute__((packed)) ttfs_directory_block {
	uint32_t file_count;
	uint32_t file_inodes[BLOCK_SIZE/sizeof(uint32_t) - 1];
} ttfs_directory_block;

#endif // TTFS_H
