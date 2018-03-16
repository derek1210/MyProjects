

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "ext2.h"
#include <string.h>
#include <math.h>
#include <libgen.h>
#include <time.h>
#include <errno.h>

unsigned char *disk;

int main(int argc, char **argv) {

  if(argc != 3) {
    fprintf(stderr, "Usage: %s <image file name> <path to file>\n", argv[0]);
        exit(1);
    }

  int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
      perror("mmap");
      exit(1);
    }
  
  readimage();

  char dest_path[EXT2_NAME_LEN]; 
  char dest[EXT2_NAME_LEN];
  strcpy(dest_path, argv[2]);
  strcpy(dest, argv[2]);

  char des_file_name[EXT2_NAME_LEN];
  strcpy(des_file_name, basename(argv[2]));
  
  char *parent_path = dirname(dest);  
  // struct ext2_dir_entry *dest_dir = find_entry(parent_path);
  struct ext2_dir_entry *dest_dir = validate_path(parent_path, NULL, EXT2_FT_DIR);
  printf("find dest dir\n");
  if (!dest_dir) {
    fprintf(stderr, "Incorrect path\n");
    return -ENOENT;
  }
  if (dest_dir->file_type != EXT2_FT_DIR) {
    fprintf(stderr, "Invalid path");
    exit(-ENOTDIR);
  }
  int* dest_parent_block = block_of_dir_inode(dest_dir->inode);
  int block_index = find_recent_block(dest_parent_block);
  int dest_block = dest_parent_block[block_index];

  struct ext2_dir_entry *dest_file = find_entry(dest_block, dest_path);
  if (dest_file) {
    fprintf(stderr, "destination file already exist: name: %s\n", dest_path);
    exit(-EEXIST);
  }
  //search the parent dir of file with find_entry
  //go though the dir
  //check rec_len 
  //if entry->rec_len > rec_len_needed
  // deleted_entry = entry + rec_len_needed .
  for (int i=0; i < 12 && dest_parent_block[i] != 0; i++) {
    struct ext2_dir_entry *deleted_file = find_deleted_file(dest_parent_block[i], des_file_name);

    //if deleted_entry is dir ==> -EISDIR
      // if deleted_entry->inode is reused (bit is 1) ==> cant recover -ENOENT
    if (!deleted_file) {
      fprintf(stderr, "No such file \n");
      exit(-ENOENT);
    } else if (inode_bitmap[deleted_file->inode-1]) {
      fprintf(stderr, "Inode is reused \n");
      exit(-ENOENT);
    } else {
      //printf("here\n");
      int del_file_inode = deleted_file->inode;
      //printf("delfileinode %d\n", del_file_inode);

      // int* del_file_block = inodes[del_file_inode-1].i_block;
      // int del_block_index = find_recent_block(del_file_block);
      //printf("o\n");
      // int exact_block = dest_parent_block[del_block_index];
      //update inode 
      //printf("still\n");
      struct ext2_inode dest_inode = inodes[del_file_inode  - 1];
      update_inode(del_file_inode - 1, dest_inode.i_links_count + 1, 0, dest_inode.i_mode, dest_inode.i_size, (int*)dest_inode.i_block);
      //printf("what\n");
      // update block bitmap inode bitmap
      inode_bits[(del_file_inode-1)/8] |= (1 << ((del_file_inode-1) % 8));
      inode_bitmap[del_file_inode-1] = 1;
      // block_bitmap[exact_block-1] = 1;
      // block_bits[(exact_block-1)/8] |= (1 << ((exact_block-1) % 8));
      //  update sb & group 
      group->bg_free_inodes_count++;
      // group->bg_free_blocks_count--;
      sb->s_free_blocks_count = group->bg_free_blocks_count;
      break;
    }
  }
    return 0; 
}

