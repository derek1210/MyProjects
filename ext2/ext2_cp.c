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
#include <sys/stat.h>
#include <math.h>
#include <errno.h>
extern unsigned char * disk;

int main(int argc, char **argv) {
  if(argc != 4) {
    fprintf(stderr, "Usage: %s <image file name> <path on native OS> <path on ext2 formatted disk>\n", argv[0]);
    exit(1);
  }
  int fd = open(argv[1], O_RDWR);

  disk = mmap(NULL, 128 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if(disk == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }
  readimage(); 
  char *path_source = argv[2];
  char *path_dest = argv[3];
  int source = open(argv[2], O_RDONLY);
  int comp_len;
  if (source < 0) {
    perror("Permission denied or file does not exist!");
    exit(-ENOENT);
  }
  // validate dest path
  struct ext2_dir_entry* dest_entry = validate_path(path_dest, path_source, EXT2_FT_REG_FILE);

  //if path to dest is invalid
  if (!dest_entry) { 
    fprintf(stderr, "Invalid path!\n");
    exit(-ENOENT);
  } 
  // valid path would return entry in either dest name or source name 
  // depends on whether dest exists as a directory or a file
  char abs_dest[EXT2_NAME_LEN];
  if (basename(path_dest)[0] == '/') {
    // want to copy into current working directory
    // add "." to enable loop
      strncpy((char*)abs_dest, ".", 1);
      strncat((char*)abs_dest, path_dest, strlen(path_dest)+1); 
  }
  else {
    // copying into deeper directory
    strncpy((char*)abs_dest, path_dest, strlen(basename(path_dest))+1);
  }
  comp_len = compare_len(strlen(basename(abs_dest)), dest_entry->name_len);

  printf("abs_dest %s\n", abs_dest);
  // check if entry has the same name as specified in path_dest
  if (strncmp(dest_entry->name, basename(abs_dest), comp_len) == 0) {
      // check if it is a file
    if (dest_entry->file_type == EXT2_FT_REG_FILE) {
        // same name file exists
        fprintf(stderr, "File already exists\n");
        exit(-EEXIST);
    }
  }
  // then check if file entry has the same name as specified in path_source
  comp_len = compare_len(strlen(basename(path_source)), dest_entry->name_len);
  if (strncmp(dest_entry->name, basename(path_source), comp_len) == 0) {
    if (dest_entry->file_type == EXT2_FT_REG_FILE) {
      // same name file exists
      fprintf(stderr, "File already exists\n");
      exit(-EEXIST);
    }
  }
  int* parent_blocks;
  int exact_block;
  int block_index;
  // not named as either, could be copying into a directory called dest_name or
  // dest_entry is a directory to be copied into 
  if (dest_entry->file_type == EXT2_FT_DIR) {   
    // look into this directory entry
    parent_blocks = block_of_dir_inode(dest_entry->inode);
    // int i;
    block_index = find_recent_block(parent_blocks);
    // block for this parent directory
    exact_block = parent_blocks[block_index];
    // since return a directory, possibly named as basename of path_dest or path_dest's direct parent
    // need to check within whether entry with same name as path_source or path_dest exists already
    
    // inside this directory, find if there is an entry named as source file
    struct ext2_dir_entry *child_entry = find_entry(exact_block, basename(path_source));
    if (child_entry) {
      // found an entry 
      // specify error conditions
      // found an entry same name as source and is a file
      if (child_entry->file_type == EXT2_FT_REG_FILE) {
        // file already exists
        int comp_len2 = compare_len(strlen(basename(abs_dest)), child_entry->name_len);
        int comp_len1 = compare_len(strlen(basename(path_source)), child_entry->name_len);
        if (strncmp(child_entry->name, basename(path_source), comp_len1) == 0) {
          comp_len = compare_len(strlen(basename(abs_dest)), child_entry->name_len);
          if (strncmp(child_entry->name, basename(abs_dest), comp_len) == 0) {
            // double check for case where path_dest is current working directory "/"
            printf("child %s\n", child_entry->name);
            fprintf(stderr, "File already exists\n");
            exit(-EEXIST);
          }
          // else safe to copy file named as path_source
        }
        else if (strncmp(child_entry->name, basename(abs_dest), comp_len2) == 0) {
          if (child_entry->file_type == EXT2_FT_REG_FILE) {
            fprintf(stderr, "File already exists\n");; //error exit 
            exit(-EEXIST);
          }
        }
        // else safe to copy into this directory
      } 
      else{
        // found a directory with same name
        fprintf(stderr, "Name occupied by other type\n");
        exit(-ENOENT);
      }
      // safe to copy into this directory
    }
  }

  struct stat st;
  stat(path_source, &st);
  int file_size = st.st_size;
  int blocks_needed = ceil(file_size/(float)1024);
  int indirect = 0;
  int direct_blocks = 0;
  int indirect_blocks = 0;

  if (blocks_needed > group->bg_free_blocks_count) {
    fprintf(stderr, "Not enough space!");
    exit(-ENOSPC);
  }
  if (blocks_needed > 12) { //need one more indirect block
    blocks_needed += 1;
    indirect = 1;
  } 
  if (indirect) {
    direct_blocks = 12;
    // need this blocks being pointed by the last (13th) block
    indirect_blocks = blocks_needed - 12;
    //printf("direct_blocks %d\n", direct_blocks);
  }
  else {
    direct_blocks = blocks_needed;
  }

  int free_inode;      
  int rec_len;
  // get this directory entry's last used block number
  int parent_inode = dest_entry->inode;
  parent_blocks =  block_of_dir_inode(parent_inode);
  block_index = find_recent_block(parent_blocks);
  // block for this parent directory
  exact_block = parent_blocks[block_index];
  // assign inode for this file
  free_inode = next_free_inode();
  // check whether we have free inode currently
  if (free_inode == -1) {
    fprintf(stderr, "Ran out of inode!\n");
    exit(-ENOSPC);
  }
  // determine whether current directory block has enough space
  // destination path defined a name for the file different than filename
  comp_len = compare_len(strlen(basename(path_dest)), dest_entry->name_len);
  if (strncmp(dest_entry->name, basename(path_dest), comp_len)) {
    if (strcmp(path_dest, "/") == 0) {// path_dest == "/" copying into root directory
      // use the original filename
      // check if this directory has enough rec_len 
      rec_len = get_rec_len(exact_block, sizeof(struct ext2_dir_entry) + strlen(basename(path_source)) -(sizeof(struct ext2_dir_entry) + strlen(basename(path_source)))%4+4);
      if (rec_len < 0) {
        // need one more block for parent directory to store its content
        exact_block = make_up_block(parent_inode, blocks_needed);
        // exact_block = next_free_block();
        // // check for availability
        rec_len = get_rec_len(exact_block, 0);
        block_bitmap[exact_block-1] = 1;
        dir_blocks[num_dir][block_index+1] = exact_block;
        block_bits[(exact_block-1)/8] |= (1 << ((exact_block-1) % 8));
        group->bg_free_blocks_count--;
      }
      rec_len = update_rec_len(exact_block, rec_len);
      create_entry(exact_block, free_inode, basename(path_source), EXT2_FT_REG_FILE, rec_len);
    } 
    else {
      // specified filename, use dest_name 
      rec_len = get_rec_len(exact_block, sizeof(struct ext2_dir_entry) + strlen(basename(path_dest))-(sizeof(struct ext2_dir_entry) + strlen(basename(path_dest)))%4+4);
          
      if (rec_len < 0) {
        rec_len = make_up_block(parent_inode, blocks_needed);
        // need one more block for parent directory to store its content
        // exact_block = next_free_block();
        // // check for availability
  
        // get remaining rec_len
        rec_len = get_rec_len(exact_block, 0);
        // new_rec_len = update_rec_len(exact_block, rec_len);
        block_bitmap[exact_block-1] = 1;
        dir_blocks[num_dir][block_index+1] = exact_block;
        block_bits[(exact_block-1)/8] |= (1 << ((exact_block-1) % 8));
        group->bg_free_blocks_count--;
      }
      //printf("nnnn\n");
      rec_len = update_rec_len(exact_block, rec_len);
      //printf("new %d\n", new);
      create_entry(exact_block, free_inode, basename(path_dest), EXT2_FT_REG_FILE, rec_len);
    }
  }
  else {
    // copying file with original filename
    //printf("hi\n");
    //printf("exact block %d\n", exact_block);
    rec_len = get_rec_len(exact_block, sizeof(struct ext2_dir_entry) + strlen(basename(path_source)) -(sizeof(struct ext2_dir_entry) + strlen(basename(path_source)))%4+4);
    //printf("cp rec_len %d\n", rec_len);
    if (rec_len < 0) {
      // need one more block for parent directory to store its content
      rec_len = make_up_block(parent_inode, blocks_needed);
      rec_len = get_rec_len(exact_block, 0);
      block_bitmap[exact_block-1] = 1;
      dir_blocks[num_dir][block_index+1] = exact_block;
      block_bits[(exact_block-1)/8] |= (1 << ((exact_block-1) % 8));
      group->bg_free_blocks_count--;
    }
    rec_len = update_rec_len(exact_block, rec_len);
    create_entry(exact_block, free_inode, basename(path_source), EXT2_FT_REG_FILE, rec_len);
  }

  int offset = 0;
  char *source_data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, source, 0); //read whole file
  int i_size = st.st_size;
  int free_block; 
  int i_block[13];
  int i;
  char *next_block; //address of next free block

  // general update
  //update inode bitmap
  inode_bits[(free_inode-1)/8] |= (1 << ((free_inode-1) % 8));
  inode_bitmap[free_inode-1] = 1;
  group->bg_free_inodes_count--;
  // direct copy into 12 direct blocks
  // copy data
  for (i = 0; i < direct_blocks; i++) {//loop until no data left
    free_block = next_free_block();
    // no need to check for availability
    // since block_needed already checked
    i_block[i] = free_block;
    // update group descriptor
    group->bg_free_blocks_count--;
    next_block = (char *) (disk + 1024 * free_block);       
    //update block bitmap
    block_bitmap[free_block-1] = 1;
    block_bits[(free_block-1)/8] |= (1 << ((free_block-1) % 8));
    //copy into disk
    if (file_size > 1024) {
      memcpy(next_block, source_data + offset, 1024);
      //next block need to be copied
      offset += 1024;
      file_size -= 1024;
    } 
    else { //last block
      memcpy(next_block, source_data + offset, file_size);    
    }
  }
  
  // store into one indirect block
  if (indirect) {
    free_block = next_free_block();
    // put indirect block into blocks array
    i_block[12] = free_block;
    // update group descriptor
    group->bg_free_blocks_count--;
    //update actual block bitmap and global blocks array
    block_bitmap[free_block-1] = 1;
    block_bits[(free_block-1)/8] |= (1 << ((free_block-1) % 8));
    int *pointer = (int* )(disk + 1024 * (free_block + 1));
    for (int j = 0; j < indirect_blocks; j++) {
      // find free block for extra data
      free_block = next_free_block();
      *pointer = free_block;
      pointer++;
      //update actual block bitmap and global blocks array
      block_bitmap[free_block-1] = 1;
      block_bits[(free_block-1)/8] |= (1 << ((free_block-1) % 8));
    }
  }
  //update inode table
  update_inode(free_inode-1, 0, 0, EXT2_S_IFREG, i_size, i_block);
  // update super block
  sb->s_free_blocks_count = group->bg_free_blocks_count;
  sb->s_free_inodes_count = group->bg_free_inodes_count;

  // close file
  if (close(source) < 0) {
    perror("Error closing file\n");
    exit(1);
  }
  return 0;
}

