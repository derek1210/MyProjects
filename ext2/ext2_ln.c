

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
#include <errno.h>

unsigned char *disk;

int main(int argc, char **argv) {

  int opt;
  int is_soft = 0;
  int image_idx = 1;
  while ((opt = getopt(argc, argv, "s:")) != -1) {
    switch (opt) {
    case 's':
      is_soft = 1;
      image_idx = 3;
      break;

    default:
      if (argc != 4) {
  fprintf(stderr, "Usage: ext2_ln <image file name> -s <absolute source file> <absolute target path>");
  exit(1);
      }
    }
  }
    
  int fd = open(argv[image_idx], O_RDWR);
  disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (disk == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  
  readimage();
  //get the source file name from argv[2]
  char source_path[EXT2_NAME_LEN];
  char source[EXT2_NAME_LEN]; 
  strcpy(source_path, argv[2]);
  strcpy(source, argv[2]);
  
  char dest_path[EXT2_NAME_LEN]; 
  char dest[EXT2_NAME_LEN];
  strcpy(dest_path, argv[3 + is_soft]);
  strcpy(dest, argv[3 + is_soft]);

  //check destination path
  char dest_file_name[EXT2_NAME_LEN];
  strcpy(dest_file_name, basename(argv[3 + is_soft]));
  
  printf("dest path%s\n",  dest_file_name);
  char *parent_path = dirname(dest);
  printf("dest parent %s\n", parent_path);
  // validate directory of destination path
  struct ext2_dir_entry *dest_dir = validate_path(parent_path, NULL, EXT2_FT_DIR);
  int *dest_parent_block;
  int dest_block;
  int block_index;
  // int i;
  if (!dest_dir) {
    fprintf(stderr, "Incorrect path\n");
    return -ENOENT;
  }
  else {
    dest_parent_block = block_of_dir_inode(dest_dir->inode);
    if (dest_dir->file_type != EXT2_FT_DIR) {
      fprintf(stderr, "Invalid path");
      exit(-ENOTDIR);
    }
    else {
      printf("aa\n");
      block_index = find_recent_block(dest_parent_block);
      dest_block = dest_parent_block[block_index];
      // for (i=0; i < 15 && dest_parent_block[i] != 0; i++) {
        // printf("h\n");
        // dest_block = dest_parent_block[i];
      printf("dest file name %s\n", dest_file_name);
      struct ext2_dir_entry *dest_file = find_entry(dest_block, dest_file_name);
      if (dest_file) {
        fprintf(stderr, "destination file already exist: name: %s\n", dest_file_name);
        exit(-EEXIST);
      }
      
    }
  }
  // struct ext2_dir_entry *dest_file = find_entry(dest_path);
  // if (dest_file) {
  //   fprintf(stderr, "destination file already exist: name: %s\n", dest_path);
  //    return -EEXIST;
  // }
  printf("start src\n");
  int *src_parent_block;
  int src_block;
  char *parent_source = dirname(source);
  struct ext2_dir_entry *src_dir = validate_path(parent_source, NULL, EXT2_FT_DIR);
  struct ext2_dir_entry *src_file;
  if (!src_dir) {
    fprintf(stderr, "Incorrect path\n");
    return -ENOENT;
  }
  else {
    src_parent_block = block_of_dir_inode(src_dir->inode);
    if (src_dir->file_type != EXT2_FT_DIR) {
      fprintf(stderr, "Invalid path");
      exit(-ENOTDIR);
    }
  }
  block_index = find_recent_block(src_parent_block);
  src_block = src_parent_block[block_index];
    // for (i=0; i < 15 && src_parent_block[i] != 0; i++) {
    //   src_block = src_parent_block[i];
  printf("source base %s\n", basename(source_path));
  // src_file does not have to exist
  src_file = find_entry(src_block, basename(source_path));
  printf("source path orig %s\n", source_path);
  // struct ext2_dir_entry *src_file = find_entry(exact_block, source_path);
  int block_dest_entry = dest_block;
  int rec_len_need = sizeof(struct ext2_dir_entry) + strlen(dest_file_name)-(sizeof(struct ext2_dir_entry) + strlen(dest_file_name))%4+4;
  int new_rec_len = get_rec_len(block_dest_entry, rec_len_need);
  struct ext2_inode dest_inode = inodes[dest_dir->inode - 1];
  //for soft link
  if (is_soft){
    //find a free block for the link
    int new_block_idx  = next_free_block();
    printf("%d\n", new_block_idx);
    if (new_block_idx == -1) {
      fprintf(stderr, "Not enough free blocks for file content\n");
      exit(1);
    }
    int new_block[1];
    new_block[0] = new_block_idx;
        //create new inode 
    int new_inode_idx = next_free_inode();
    if (new_inode_idx == -1){
      fprintf(stderr, "No inodes available");
      exit(1);
    }
    struct ext2_inode dest_inode = inodes[new_inode_idx- 1];
    printf("size %d, block %d mode: %d\n", dest_inode.i_size, dest_inode.i_block[1], dest_inode.i_mode);
    printf("root block %d\n", inodes[12].i_block[0]);
    printf("update inode bitmap\n");
    // struct ext2_inode dest_inode = inodes[dest_dir->inode - 1];
    if (new_rec_len == -1) { //dest dir doesnt have enough space
      // find a free block for the directory
      block_dest_entry = make_up_block(dest_dir->inode, 1);
      new_rec_len = get_rec_len(block_dest_entry, 0);
      // int parent_block = next_free_block();
      // if (parent_block == -1) {
      //   fprintf(stderr, "Not enough free blocks for file content\n");
      //   exit(-ENOSPC);
      // }
      // // new_rec_len = update_rec_len(parent_block, new_rec_len);
      // //assign a new block for dest_dir
      update_inode(dest_dir->inode-1, dest_inode.i_links_count + 1, dest_inode.i_dtime, dest_inode.i_mode, dest_inode.i_size + EXT2_BLOCK_SIZE, &block_dest_entry);
      
      // create_entry(parent_block, new_inode_idx, dest_file_name, EXT2_FT_REG_FILE, EXT2_BLOCK_SIZE); //create entry for dest_file cover the whole block's rec_len
      // update_inode(new_inode_idx - 1, 0, 0, EXT2_S_IFLNK, strlen(source_path), new_block);
      // //update block bitmap
      // block_bitmap[parent_block-1] = 1;
      // block_bits[(parent_block-1)/8] |= (1 << ((parent_block-1) % 8));
      dir_blocks[num_dir][block_index+1] = block_dest_entry;
      block_bitmap[block_dest_entry-1] = 1;
      block_bits[(block_dest_entry-1)/8] |= (1 << ((new_block_idx-1) % 8));
      group->bg_free_blocks_count--;
    } 
    // else {
    // update inode table information of this new 
    update_inode(new_inode_idx - 1, 0, 0, EXT2_S_IFLNK, strlen(source_path), new_block);
      
    //update inode bitmap
    inode_bits[(new_inode_idx -1)/8] |= (1 << ((new_inode_idx-1) % 8));
    inode_bitmap[new_inode_idx-1] = 1;
    group->bg_free_inodes_count--;
      //update block bitmap
    block_bitmap[new_block_idx-1] = 1;
    block_bits[(new_block_idx-1)/8] |= (1 << ((new_block_idx-1) % 8));
    group->bg_free_blocks_count--;
    printf("meeeeeeeeeeeeeeem\n");
    memcpy((char *) (disk + 1024 * (new_block_idx + 1)), source_path, sizeof(source_path));
        
    printf("create block\n");
    //create new dir entry
    new_rec_len = update_rec_len(block_dest_entry, new_rec_len);
    create_entry(block_dest_entry, new_inode_idx, dest_file_name, EXT2_FT_SYMLINK, new_rec_len); 
    // }
  }
  else{ 
    //for hard link
    //check if source is valid
    if (!src_dir) {
      fprintf(stderr, "Incorrect path\n");
      return -ENOENT;
    }
    if (!src_file) {
      fprintf(stderr, "No such file\n");
      return -ENOENT;
    } else if (src_file->file_type == EXT2_FT_DIR) {
      fprintf(stderr, "Not a file \n");
      return -EISDIR;
    }
    //create a new directory entry
    struct ext2_inode src_inode = inodes[src_file->inode - 1];
    // struct ext2_inode dest_inode = inodes[dest_dir->inode - 1];
    if (new_rec_len == -1) { //dest dir doesnt have enough space
      block_dest_entry = make_up_block(dest_dir->inode, 1);
      // int free_block = next_free_block();
      // if (free_block == -1) {
      //   fprintf(stderr, "Not enough free blocks for file content\n");
      //   exit(-ENOSPC);
      // }
      new_rec_len = get_rec_len(block_dest_entry, 0);
      // //assign a new block for dest_dir
      update_inode(dest_dir->inode-1, dest_inode.i_links_count + 1, dest_inode.i_dtime, dest_inode.i_mode, dest_inode.i_size + EXT2_BLOCK_SIZE, &block_dest_entry);
      // create_entry(free_block, src_file->inode, dest_file_name, EXT2_FT_REG_FILE, new_rec_len); //create entry for dest_file
      //update block bitmap
      block_bitmap[block_dest_entry-1] = 1;
      dir_blocks[num_dir][block_index+1] = block_dest_entry;
      block_bits[(block_dest_entry-1)/8] |= (1 << ((block_dest_entry-1) % 8));
      group->bg_free_blocks_count--;
    }
      new_rec_len = update_rec_len(block_dest_entry, new_rec_len);
      update_inode(src_file->inode - 1, src_inode.i_links_count + 1, src_inode.i_dtime, src_inode.i_mode, src_inode.i_size, NULL);
      create_entry(block_dest_entry, src_file->inode, dest_file_name, EXT2_FT_REG_FILE, new_rec_len); //create entry for dest_file

  }
  sb->s_free_blocks_count = group->bg_free_blocks_count;
  sb->s_free_inodes_count = group->bg_free_inodes_count;
  return 0;
}



