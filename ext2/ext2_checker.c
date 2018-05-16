

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include <time.h>
#include "ext2.h"
#include <math.h>

unsigned char *disk;

int main(int argc, char **argv) {
	if(argc != 2) {
    fprintf(stderr, "Usage: %s <image file name>\n", argv[0]);
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    readimage();
    int num_fixes = 0;
    //check superblock and group counters
    int free_blocks = 0;
    int free_inodes = 0;
    for (int j = 0; j < 127; j++) {
    	if (block_bitmap[j] == 0) {
      		free_blocks++;
    	}
  	}
  	for (int i = 0; i < 32; i++) {
	    if(inode_bitmap[i] == 0) {
	    	free_inodes++;
	    }
	}
	int diff;
  	if (sb->s_free_blocks_count != free_blocks) {
  		diff = abs(sb->s_free_blocks_count - free_blocks);
  		sb->s_free_blocks_count = free_blocks;
  		num_fixes += diff;
  		printf("Fixed: superblock's free blocks counter was off by %d compared to the bitmap\n", diff);
  	}
  	if (sb->s_free_inodes_count != free_inodes) {
  		diff = abs(sb->s_free_inodes_count - free_inodes);
  		sb->s_free_inodes_count = free_inodes;
  		num_fixes += diff;
  		printf("Fixed: superblock's free inodes counter was off by %d compared to the bitmap\n", diff);
  	}
  	if (group->bg_free_blocks_count != free_blocks) {
  		diff = abs(group->bg_free_blocks_count-free_blocks);
  		group->bg_free_blocks_count = free_blocks;
  		num_fixes += diff;
  		printf("Fixed: block group's free blocks counter was off by %d free blocks compared to the bitmap\n", diff);
  	}
  	if (group->bg_free_inodes_count != free_inodes) {
  		diff = abs(group->bg_free_inodes_count-free_inodes);
  		group->bg_free_inodes_count = free_inodes;
  		num_fixes += diff;
  		printf("Fixed: block group's free inodes counter was off by %d compared to the bitmap\n", diff);
  	}
    //check inode's mode matches the file type 
  	struct ext2_dir_entry *entry;
  	struct ext2_inode inode_num;
	for (int i = 0; i < 32 && ((i == EXT2_ROOT_INO-1) || (i >= 11)); i++) {
		inode_num = inodes[i];
		entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * inode_num.i_block[0]);
		if (EXT2_S_IFREG & inode_num.i_mode) {
			if (entry->file_type != 'f') {
				entry->file_type = 'f';
				printf("Fixed: Entry type vs inode mismatch: inode [%d]\n", i + 1);	
				num_fixes++;		
			}
		} else if (EXT2_S_IFDIR & inode_num.i_mode) {
			if (entry->file_type != 'd') {
				entry->file_type = 'd';
				printf("Fixed: Entry type vs inode mismatch: inode [%d]\n", i + 1);
				num_fixes++;
			}
		} else if (EXT2_S_IFLNK & inode_num.i_mode){
			if (entry->file_type != '0') {
				entry->file_type = '0';
				printf("Fixed: Entry type vs inode mismatch: inode [%d]\n", i + 1);
				num_fixes++;
			}
		}
  	}
  	int in_use, j;
    //check inode bitmaps every file/dir/link must have its inode = 1
    for (j = 0; j < 32; j++) {
	    in_use = inode_bitmap[j];
		if (j < 11) {
			if (!in_use) {
			    inode_bits[j/8] |= (1 << (j % 8));
			    inode_bitmap[j] = 1;
			    sb->s_free_inodes_count--;
				group->bg_free_inodes_count--;
				printf("Fixed: inode [%d] not marked as in-use\n", j + 1);
			}
		} else {
			inode_num = inodes[j];
			entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * inode_num.i_block[0]);
			if ((EXT2_S_IFREG & inode_num.i_mode) || (EXT2_S_IFDIR & inode_num.i_mode) || (EXT2_S_IFLNK & inode_num.i_mode)){
				if (entry) {
					if (!in_use) {
						for (int k=0; k < inode_num.i_blocks/2 && inode_num.i_block[k] != 0; k++) {
							struct ext2_dir_entry *in_use_entry = find_entry(inode_num.i_block[k], entry->name);
							//if the entry has not been removed. 
							if (in_use_entry) { 
								inode_bits[j/8] |= (1 << (j % 8));
								inode_bitmap[j] = 1;
								sb->s_free_inodes_count--;
								group->bg_free_inodes_count--;
								printf("Fixed: inode [%d] not marked as in-use\n", j + 1);
								num_fixes++;
								break;
							} 
						}
					}	
				}
			}
		}
	}
    //check every entry's dtime = 0
    for (j = 0; j < 32; j++) {
    	inode_num = inodes[j];
	    in_use = inode_bitmap[j];
		if (j < 11) {
			if (inode_num.i_dtime) {
			    inode_num.i_dtime = 0;
			}
		} else if (in_use) {
			entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * inode_num.i_block[0]);
			if ((EXT2_S_IFREG & inode_num.i_mode) || (EXT2_S_IFDIR & inode_num.i_mode) || (EXT2_S_IFLNK & inode_num.i_mode)){
				if (entry) {
					if (inode_num.i_dtime) {
						inodes[j].i_dtime = 0;
						printf("Fixed: valid inode marked for deletion: [%d]\n", j + 1);
						num_fixes++;
					}
				}
			}
		}
    }

    //check block bitmap, each used block must have bit = 1
    int block_bit_fix;
    int k;
    for (j = 0; j < 32; j++) {
    	if (j == 1 || j >= 10) {
	    	inode_num = inodes[j];
		    in_use = inode_bitmap[j];
		    block_bit_fix = 0;
		    if (in_use) {
		    	entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * inode_num.i_block[0]);
				if ((EXT2_S_IFREG & inode_num.i_mode) || (EXT2_S_IFDIR & inode_num.i_mode) || (EXT2_S_IFLNK & inode_num.i_mode)) {
					if (entry) {
						for (k = 0; k < inode_num.i_blocks/2; k++) {
							if (!block_bitmap[inode_num.i_block[k] - 1]) {
								block_bits[(inode_num.i_block[k]-1)/8] |= (1 << ((inode_num.i_block[k]-1)) % 8);
								block_bitmap[inode_num.i_block[k]-1] = 1;
								block_bit_fix++;
								num_fixes++;		
								group->bg_free_blocks_count--;
								sb->s_free_blocks_count--;
							}
						}
						//if there is indirect block, check the rest
						if (inode_num.i_blocks/2 > 12) { 
							int rest = inode_num.i_blocks/2 - 13;   
							for (k = 0; k < rest; k++) {
								int *indirect = (int *)(disk + EXT2_BLOCK_SIZE * inode_num.i_block[12]);
								if (!block_bitmap[indirect[k] - 1]) {
									block_bits[(inode_num.i_block[k]-1)/8] |= (1 << ((inode_num.i_block[k]-1)) % 8);
									block_bitmap[inode_num.i_block[k]-1] = 1;
									block_bit_fix++;
									num_fixes++;		
									group->bg_free_blocks_count--;
									sb->s_free_blocks_count--;
								}
							}
						}
						if (block_bit_fix) {
							printf("Fixed: %d in-use data blocks not marked in data bitmap for inode: [%d]\n", block_bit_fix, j + 1);
						}
					}
				}
			}
		}
	}
    if (num_fixes) {
    	printf("%d file system inconsistencies repaired!\n", num_fixes);
    } else {
    	printf("No file system inconsistencies detected!\n");
    }
    return 0;
}

