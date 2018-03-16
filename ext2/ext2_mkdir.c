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
extern unsigned char * disk;

int main(int argc, char **argv) {
  if(argc != 3) {
    fprintf(stderr, "Usage: %s <image file name> <path>\n", argv[0]);
        exit(1);
    }
    int fd = open(argv[1], O_RDWR);

    disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(disk == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    
    char *path = argv[2];
    //char *dir_name = basename(path);
    readimage(); 
    // int i;
    int comp_len;
    int free_inode;
    int free_block;
    int *parent_blocks;   
    int parent_inode;
    int exact_block;
    int block_index;
    // find the directory entry with given name in given path
    struct ext2_dir_entry *parent_entry = validate_path(path, NULL, EXT2_FT_DIR);

    // destination exists
    if (parent_entry != NULL) {
      // check for same name file
      comp_len = compare_len(strlen(basename(path)), parent_entry->name_len);
      if (strncmp(parent_entry->name, basename(path), comp_len) == 0) {
        fprintf(stderr, "directory already existed\n"); //error exit 
        exit(-EEXIST);
      }
      // safe to make directory in parent directory
      // 
      parent_blocks = block_of_dir_inode(parent_entry->inode);
      block_index = find_recent_block(parent_blocks);
      // block for parent directory
      exact_block = parent_blocks[block_index];
        // for (i=0; i < 15 && parent_block[i] != 0; i++) {
        //   exact_block = parent_block[i];
      parent_inode = parent_entry->inode;
      free_inode = next_free_inode(); 
      // block for requested directory
      free_block = next_free_block();
      // check for available inode
      if (!(free_inode == -1)){
        // check for available block
        if (free_block == -1) {
          fprintf(stderr, "Not enough free blocks for file content\n");
          exit(-ENOSPC);
        }
        // update inode array and disk inode bitmap
        inode_bitmap[free_inode-1] = 1;
        inode_bits[(free_inode-1)/8] |= (1 << ((free_inode-1) % 8));
        // update directory inodes bitmap
        dir_inodes[num_dir] = free_inode;
        // update dir block used
        block_bitmap[free_block-1] = 1;
        dir_blocks[num_dir][block_index] = free_block;
            // for (i = 0; i < 15 && dir_blocks[num_dir] == 0; i++) {
            //  dir_blocks[num_dir][i] = free_block;
            // }
        // update disk block bitmap
        block_bits[(free_block-1)/8] |= (1 << ((free_block-1) % 8));
        //update number of directory
        num_dir++;

        char curr[2] = ".";
        char parent[3] = "..";
        int free_blocks[2];
        free_blocks[0] = free_block;
        // get potential remaining rec_len in this directory block
        int old_rec_len = get_rec_len(exact_block, 0); 
        // update previous entry's rec_len 
        int new_rec_len = update_rec_len(exact_block, old_rec_len);
        
        // printf("new_rec_len %d\n", )
        // not enough space for new entry
        if (old_rec_len == -1) {
          // assign a new block for this
          exact_block = make_up_block(parent_inode, 1);
          // exact_block = next_free_block();
          // if (exact_block == -1) {
          //   fprintf(stderr, "Not enough free blocks for file content\n");
          //   exit(-ENOSPC);
          // }
          // // update the size of this directory
          // update_inode(parent_inode, inodes[parent_inode-1].i_links_count, 0, EXT2_S_IFDIR, EXT2_BLOCK_SIZE + inodes[parent_inode-1].i_size, &exact_block);
          // // create entry points to parent in this directory
          // create_entry(exact_block, parent_inode, curr, EXT2_FT_DIR, EXT2_BLOCK_SIZE);
          // get remaining rec_len
          old_rec_len = get_rec_len(exact_block, 0);
          // // int parent_rec_len = update_rec_len(exact_block, 0);
          // // // create entry of  directory
          // // create_entry(exact_block, parent_inode, parent, EXT2_FT_DIR, parent_rec_len);
          // // free_blocks[1] = new_block;
          new_rec_len = update_rec_len(exact_block, old_rec_len);
          //
          block_bitmap[exact_block-1] = 1;
          dir_blocks[num_dir][block_index+1] = exact_block;
          block_bits[(exact_block-1)/8] |= (1 << ((exact_block-1) % 8));
          group->bg_free_blocks_count--;
        }

        // printf("exact block %d\n", exact_block);
        // create entry for requested directory on found block
        create_entry(exact_block, free_inode, basename(path), EXT2_FT_DIR, new_rec_len);

        // create entry points to requested directory on its own allocated block
        create_entry(free_block, free_inode, curr, EXT2_FT_DIR, EXT2_BLOCK_SIZE);
        // get remaining rec_len for parent entry
        int parent_rec_len = get_rec_len(free_block, 0);
        int new_parent_len = update_rec_len(free_block, parent_rec_len);
        // printf("parent_rec_len %d\n", parent_rec_len);
        // printf("new %d\n", new_parent_len);
        // create entry points to parent of requested directory on its own block
        create_entry(free_block, parent_inode, parent, EXT2_FT_DIR, new_parent_len);
        // printf("free %d\n", free_block);
        
        // add information to newly allocated inode for this requested directory
        update_inode(free_inode-1,0,0,EXT2_S_IFDIR, EXT2_BLOCK_SIZE,free_blocks);
        // increment links_count on parent directory 
        update_link_count(exact_block);
        
        // update group descriptor
        group->bg_free_inodes_count--;
        group->bg_free_blocks_count--;
        group->bg_used_dirs_count++;
        // update super block
        sb->s_free_blocks_count = group->bg_free_blocks_count;
        sb->s_free_inodes_count = group->bg_free_inodes_count;

      } 
    }
    else {
      fprintf(stderr, "Invalid path!\n");
      exit(-ENOENT);
    }
    return 0;
}




