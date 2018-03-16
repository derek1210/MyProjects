#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"
#include <string.h>
#include <libgen.h>
#include <errno.h>
#include <time.h>
int main(int arg, char** argv) {
	if(arg != 3) {
        fprintf(stderr, "Usage: %s <image file name> <path>\n", argv[0]);
        exit(1);
    }
    // BONUS: "-r" flag after disk_name allows removing DIR
    	// recursively remove all the contents of the directory
    	// ignored when used with a file/link (i.e. regular remove)
 //    int opt;
	// while ((opt = getopt(argc, argv, "r:")) != -1) {
	// 	switch (opt) {
	// 	case 'r':
			
	// 		break;
	// 	default:
	// 		if(argc != 3) {
 //        	fprintf(stderr, "Usage: %s <image file name>\n", argv[0]);
 //        	exit(1);
 //    		}
	// 	}
	// }
    
    int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    char *path = argv[2]; // file or link
    // char *dir_name = basename(path);
    char *path_copy = strdup(path);
    readimage(); 
    // find if its parent path exists
    struct ext2_dir_entry *parent_entry = validate_path(dirname(path), NULL, EXT2_FT_DIR);
    if (parent_entry == NULL) {
        fprintf(stderr, "Invalid path!\n");
        exit(-ENOENT);
    }
    printf("parent\n");
    int old_rec_len = 0;
    // int file_size;
    int rec_len = 0;
    int inode_index;
    int* parent_blocks = block_of_dir_inode(parent_entry->inode);
    int block_index = find_recent_block(parent_blocks);
    // block for parent directory
    int exact_block = parent_blocks[block_index];
    // for (int i=0; i < 12 && parent_block[i] !=0; i++) {
    printf("basename %s\n", basename(path_copy));
    struct ext2_dir_entry *curr_entry = find_entry(exact_block, basename(path_copy));
    
    // found the corresponding file
    if (curr_entry) {
        // printf("basename %s\n", basename(path));
        // return rec_len to the previous entry
        // rec_len = get_rec_len(parent_block[i], 0, basename(path));
        // printf("curr rec_len %d\n", rec_len);
        if (curr_entry->file_type == EXT2_FT_REG_FILE || curr_entry->file_type == EXT2_FT_SYMLINK) {
            // file in the middle of entry case
            // file_size set to 0
            // store a copy
            // handled middle case
            // unsigned char type;
                
            old_rec_len = curr_entry->rec_len;
            printf("old_rec_len %d\n", old_rec_len);
            inode_index = curr_entry->inode-1;
            // determine whether it has a hardlink
            printf("original %d\n", inodes[inode_index].i_links_count);
            if (inodes[inode_index].i_links_count - 1 >= 1) {
                // hardlinks
                inodes[inode_index].i_links_count--;
                printf("first time %d\n", inodes[inode_index].i_links_count);
                // determine whether it is the only entry in the directory block
                if (old_rec_len == EXT2_BLOCK_SIZE) {
                    // only entry in the directory block
                    curr_entry->inode = 0;
                    // free the directory block
                    block_bitmap[exact_block-1] = 0;
                    block_bits[(exact_block-1)/8] &= ~(1 << ((exact_block-1) % 8));
                    // update the size of this directory
                    update_inode(inode_index, 0, 0, EXT2_S_IFDIR, inodes[inode_index].i_size + EXT2_BLOCK_SIZE, NULL);
                    curr_entry->inode = 0;
                    // free inodes
                    inode_bitmap[inode_index] = 0;
                    inode_bits[(inode_index)/8] &= ~(1 << ((inode_index) % 8));
                    group->bg_free_inodes_count++;
                }
                else {
                    // update inodes bitmap and table
                    // printf("inode index %d\n", inode_index);
                    // gap in inode bitmap cause inode
                    // inode_bitmap[inode_index] = 0;
                    // inode_bits[(inode_index)/8] &= ~(1 << ((inode_index) % 8));
                    if (curr_entry->file_type == EXT2_FT_REG_FILE) {
                        update_inode(inode_index, 0, time(NULL), EXT2_S_IFREG, inodes[inode_index].i_size, (int*)inodes[inode_index].i_block);
                    }
                    else {
                        update_inode(inode_index, 0, time(NULL), EXT2_S_IFLNK, inodes[inode_index].i_size, (int*)inodes[inode_index].i_block);
                    }
                    // update_inode(inode_index, inodes[inode_index].i_links_count, time(NULL), type, inodes[inode_index].i_size, (int*)inodes[inode_index].i_block);
                    rec_len = update_remove(exact_block, basename(path_copy));
                    // printf("lll\n");
                    printf("final %d\n", inodes[inode_index].i_links_count);
                        
                    // curr_entry->inode = 0;
                    printf("there\n");
                    // handle bottom case
                    // rec_len = update_rec_len(parent_block[i], rec_len, 0);
                    // double check
                    if (rec_len != old_rec_len) {
                        fprintf(stderr, "Fail to update rec_len!\n");
                        exit(1);
                    }
                }
            }
            else {
                // not hardlink
                // update inode table and bitmap
                inode_bitmap[inode_index] = 0;
                inode_bits[(inode_index)/8] &= ~(0 << ((inode_index) % 8));
                group->bg_free_inodes_count++;
                if (old_rec_len == EXT2_BLOCK_SIZE) {
                    // it is the only reference to these data blocks but also the only entry
                    // should free the data block
                    // not recoverable
                    for (int i = 0; i < 14; i++) {
                        inodes[inode_index].i_block[i] = 0;
                    }
                    // group->bg_free_inodes_count++;
                }
                else {
                    // update inodes bitmap and table
                    // printf("inode index %d\n", inode_index);
                    // gap in inode bitmap cause inode
                    // inode_bitmap[inode_index] = 0;
                    // inode_bits[(inode_index)/8] &= ~(1 << ((inode_index) % 8));
                    // group->bg_free_inodes_count++;
                    if (curr_entry->file_type == EXT2_FT_REG_FILE) {
                        update_inode(inode_index, 0, time(NULL), EXT2_S_IFREG, inodes[inode_index].i_size, (int*)inodes[inode_index].i_block);
                    }
                    else {
                        update_inode(inode_index, 0, time(NULL), EXT2_S_IFLNK, inodes[inode_index].i_size, (int*)inodes[inode_index].i_block);
                    }
                        // update_inode(inode_index, inodes[inode_index].i_links_count, time(NULL), type, inodes[inode_index].i_size, (int*)inodes[inode_index].i_block);
                    rec_len = update_remove(exact_block, basename(path_copy));
                    // printf("lll\n");
                    printf("final %d\n", inodes[inode_index].i_links_count);
                        
                    // curr_entry->inode = 0;
                    printf("here\n");
                    // handle bottom case
                    // rec_len = update_rec_len(parent_block[i], rec_len, 0);
                    // double check
                    printf("update rec_len %d\n", rec_len);
                    if (rec_len != old_rec_len) {
                        fprintf(stderr, "Fail to update rec_len!\n");
                        exit(1);
                    }
                }
            }

        }

        if (curr_entry->file_type == EXT2_FT_DIR) {
            fprintf(stderr, "Can not remove a directory!\n");
            exit(-EISDIR);
        }
            
        printf("rec_len %d\n", rec_len);
    }
    else {
        fprintf(stderr, "File does not exist!\n");
        exit(-ENOENT);
    }
}


