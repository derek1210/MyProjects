

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
void readimage() {

    sb = (struct ext2_super_block *)(disk + 1024);
    group = (struct ext2_group_desc *) (disk + 1024 * 2);
    block_bits = (unsigned char *) (disk + 1024 * group->bg_block_bitmap); 
    inode_bits = (unsigned char *) (disk + 1024 * group->bg_inode_bitmap); 
    inodes = (struct ext2_inode *)(disk + 1024 * group->bg_inode_table); //inode_table
    int byte, bit;
    unsigned char in_use;
    //Block bitmap:
      for (byte = 0; byte < 128/8; byte++) {
        for (bit = 0; bit < 8; bit++) {
    in_use = (unsigned char) ((block_bits[byte] & (1 << bit)) >> bit);
    block_bitmap[bit + byte * 8] = in_use;
        }
      }

      //Inode bitmap:
      for (byte = 0; byte < 32/8; byte++) {
        for (bit = 0; bit < 8; bit++) {
    in_use = (unsigned char) ((inode_bits[byte] & (1 << bit)) >> bit);
    inode_bitmap[bit + byte * 8] = in_use;
        }
      }
      
      //Inodes:
      int node_num;
      char type;
      int k=0;
      
      for (byte = 0; byte < 32/8; byte++) {
        for (bit = 0; bit < 8; bit++) {
    node_num = bit + byte * 8;
    in_use = (unsigned char)(inode_bits[byte] & (1 << bit));
    if (in_use) {
        
      if (node_num == (EXT2_ROOT_INO - 1) || (node_num >= EXT2_GOOD_OLD_FIRST_INO)) {
        if (S_ISREG(inodes[node_num].i_mode)) {
          type = 'f';
        } else if (S_ISDIR(inodes[node_num].i_mode)) {
          type = 'd';
        } else {
          type = '0';
        }
        int i;
         int num_block = 0;
       for (i = 0; i < inodes[node_num].i_blocks && i < 15 && inodes[node_num].i_block[i] != 0; i++) {
              if (type == 'd') {
                dir_blocks[k][num_block] = inodes[node_num].i_block[i];
                dir_inodes[k] = node_num+1;
                k++;
                num_block++;
              }
            }
      }
    }
        }
      }
      num_dir = k;
          int m = 0;
     //Directory Blocks:
    for (int j=0; j < k && m < 128 && dir_blocks[j][m]; j++){
          
      struct ext2_dir_entry *entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * dir_blocks[j][m]);
      int offset = EXT2_BLOCK_SIZE;
      while (offset != 0) {
        offset = offset - entry->rec_len;
        entry = (struct ext2_dir_entry *)((char*)entry + entry->rec_len);
      }
      m++;
    }
}

struct ext2_dir_entry *find_entry(int block_num, char* entry_name) {
  struct ext2_dir_entry *entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * block_num);
  //struct ext2_dir_entry *parent = entry;
  int offset = EXT2_BLOCK_SIZE;
  
  while (offset != 0) {
    
    offset = offset - entry->rec_len;
      printf("input name%s\n", entry_name);
      printf("comparing with %.*s\n", entry->name_len, entry->name);
      printf("offset %d\n", offset);
      int comp_size = compare_len(strlen(entry_name), entry->name_len);
      // if (entry->name_len > comp_size) {
      //   comp_size = entry->name_len;
      // }
      printf("comp size %d\n", comp_size);
      if (strncmp(entry->name, entry_name, comp_size) == 0) { // same name is found
         //if (entry->file_type == file_type) { //dir with same name 
          // printf("comp\n");
          printf("same\n");
           return entry;
      }
      entry = (struct ext2_dir_entry *)((char*)entry + entry->rec_len);
  }
    return NULL;
  
}
int compare_len(int len1, int len2) {
  if (len1 > len2) {
    return len1;
  }
  return len2;
}
int* block_of_dir_inode(int num)  {
  int i;

  for (i = 0; i < num_dir; i++) {

      // find block
      // num is an inode number
        if (dir_inodes[i] == num) {
          // return the array
          return dir_blocks[i];
        }
    
  }
  return NULL;
}
// return the index most recently used blocks by this directory inode
int find_recent_block(int* blocks_array) {
  // since we are only storing 12 direct blocks and 1 indirect block
  int recent = 11;
  for (int i = 0; i < 12; i++) {
    if (blocks_array[i] == 0) {
      // i is not used, return the block before this index
      recent = blocks_array[i-1];
    }
  }
  return recent;
}

int get_rec_len(int block_num, int rec_len_need) {
  // update previous entry's rec_len
  // and get the remaining rec_len in this block
  struct ext2_dir_entry *entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * block_num);
  int offset = 0;
  int rec_len;
  while (offset != EXT2_BLOCK_SIZE) {
    rec_len = entry->rec_len;
    offset = offset + rec_len;
    entry = (struct ext2_dir_entry *)((char*)entry + entry->rec_len);
  }
  printf("rec_len: %d \n", rec_len);
  struct ext2_dir_entry * previous_entry = (struct ext2_dir_entry *)((char *)entry - rec_len);
  int new_rec_len = sizeof(struct ext2_dir_entry) + previous_entry->name_len-(sizeof(struct ext2_dir_entry) + previous_entry->name_len)%4+4;
  printf("rec_len_need %d\n", rec_len_need);
  printf("compute rec_len %d\n", new_rec_len);
  if (rec_len_need > rec_len - new_rec_len) {
    return -1;
  }
  // previous_entry->rec_len = new_rec_len;
  //struct ext2_dir_entry *new_entry = (struct ext2_dir_entry *)((char *)previous_entry+previous_entry->rec_len);
  //new_entry->rec_len = rec_len - new_rec_len;
  printf("get %d\n", offset-rec_len);
  return offset-rec_len;       
}
int update_rec_len(int block_num, int rec_len) {
  // this goes to the entry to be modified
  struct ext2_dir_entry *entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * block_num + rec_len);
  // initially remaining rec_len
  // int offset = EXT2_BLOCK_SIZE - rec_len;
  printf("no seg fault\n");
  printf("enter entry %s\n", entry->name);
  printf("input rec_len %d\n", rec_len);
  int new_rec_len;
  // int i = 0;
    // previous entry's rec_len
   int i = entry->rec_len;
    printf("i %d\n", i);
    new_rec_len = sizeof(struct ext2_dir_entry) + entry->name_len-(sizeof(struct ext2_dir_entry) + entry->name_len)%4+4;
    entry->rec_len = new_rec_len;
    printf("new rec %d\n", new_rec_len);
    printf("output %d\n", i - new_rec_len); //980(a) 1012(..)
    // printf("return\n");
    // return remaining rec_len for incoming file
    return i - new_rec_len;
  return 0;
}
// assign one more block for the direcotry with given inode
// and create an entry point to its self
// then return new found block
int make_up_block(int inode, int blocks_needed) {
  char curr[2] = ".";
  int exact_block = next_free_block();
  // check for availability
  if (blocks_needed+1 > group->bg_free_blocks_count) {
    fprintf(stderr, "Not enough free blocks for file content\n");
    exit(-ENOSPC);
  }
  // update the size of this directory
  update_inode(inode-1, inodes[inode-1].i_links_count, 0, EXT2_S_IFDIR, EXT2_BLOCK_SIZE + inodes[inode-1].i_size, &exact_block);
  // create entry points to parent in this directory
  create_entry(exact_block, inode, curr, EXT2_FT_DIR, EXT2_BLOCK_SIZE);
  // get remaining rec_len
  // int rec_len = get_rec_len(exact_block, 0);
  // group->bg_free_blocks_count--;
  return exact_block;
}

//create a new entry to the tail of previous directory entry
void create_entry(int block_num, int inode_num, char* dir_name, unsigned char file_type, int rec_len){
  struct ext2_dir_entry *new_entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * block_num + EXT2_BLOCK_SIZE - rec_len);
  new_entry->rec_len = rec_len;
  new_entry->inode = inode_num;
  new_entry->name_len = strlen(dir_name);
  new_entry->file_type = file_type;
  
  strncpy(new_entry->name, dir_name, strlen(dir_name));

}

void update_inode(int inode, int links_count, int dtime, unsigned short mode, int size, int* i_block) {

  inodes[inode].i_mode |= mode;
  inodes[inode].i_dtime = dtime;
  
  if (links_count ==0) {
    if (inodes[inode].i_mode & EXT2_S_IFDIR) {
      inodes[inode].i_links_count = 2;
    } else {
      inodes[inode].i_links_count = 1;
    }
  } else {
     inodes[inode].i_links_count = links_count;
  }
  
  int blocks_needed = ceil(size/(float)1024);

  if (blocks_needed > 12) {
    inodes[inode].i_blocks = (blocks_needed + 1)*2;
  }
  else {
    inodes[inode].i_blocks = ceil(size/(float)512);
    printf("iblocks: %d\n", inodes[inode].i_blocks);
  }
  inodes[inode].i_size = size;


  if (i_block) {
    int j = 0;
    int seen = 0;
    int num_block = ceil((inodes[inode].i_blocks)/(float)2);
    for (int i=0;i<12 && j<num_block;i++){  
      //printf("index 0 %d\n", inodes[inode].i_block[0]);
      //printf("inside %d\n", !inodes[inode].i_block[i]);
      //printf("input %d\n", !i_block[i]);
      if (inodes[inode].i_block[i] == i_block[j]) {
        seen = 1;
      }
      if (!inodes[inode].i_block[i] && i_block[i] && i_block[i]) {
        //printf("index %d\n", i);
        //printf("origin: %d\n", inodes[inode].i_block[i]);
        if (seen == 0) {
          inodes[inode].i_block[i] = i_block[j]; 
        }
        //printf("block %d\n", inodes[inode].i_block[i]);
        j++;
      }
    }
  }
    printf("update inode: %d, link_count: %d, size: %d\n", inode, inodes[inode].i_links_count,size); 
}

//check until found the last level directory of path, not including basename
// check if a file or directory named as basename exist, if existed, return ENOENT;
// if path is valid and file or directory with the same name DNE, return last level entry
struct ext2_dir_entry * validate_path(char* dest, char* src, unsigned char file_type) {
  // check if dest is absolute
  if (dest[0] != '/') {
    // not absolute dest
    fprintf(stderr, "Invalid path!\n");
    exit(ENOENT);
  }
  //add root '.' to enable loop
  char *path = malloc(sizeof(char) * EXT2_NAME_LEN);
  printf("%s\n", path);
  strncpy(path, ".", 1);
  strncat(path, dest, strlen(dest)+1); 
  printf("path %s\n", path);
  int *parent_block;
  int found;
  const char slash[2] = "/";
  char *curr_dir = ".";
  
  char *name = basename(path); 
  char dir_name[EXT2_NAME_LEN];
  strncpy(dir_name, name, EXT2_NAME_LEN); 
  printf("name %s\n", dir_name);
  char *parent_path = dirname(path); 
  printf("parent %s\n", parent_path);
  int i;
  int comp_len = compare_len(strlen(parent_path), strlen(dir_name));
  // if dirname == dir_name == "/" => want to add to the root direcotry
  if (strncmp(parent_path, dir_name, comp_len) != 0) {
  // has more dir entry to track
  // simply extract its first level
    curr_dir = strtok(parent_path, slash); 
    // printf("curr_dir %s\n", curr_dir);
  } 
  //if dest == /  == root directory
  else if (!strcmp(dest, "/")) { 
    struct ext2_dir_entry *curr_entry =  find_entry(dir_blocks[0][0], curr_dir); // root entry EXT2_ROOT_INO 
    int *parent_block = block_of_dir_inode(curr_entry->inode); 
    struct ext2_dir_entry *child_entry;
    // find the entry with src name
    for (i=0; i < 15 && parent_block[i] != 0; i++) {
      child_entry = find_entry(parent_block[i], basename(src));
      if (!child_entry){
        // if not, return root entry
        return curr_entry; // return this dir 
      } 
    }
    return child_entry;
  }
  
  struct ext2_dir_entry *curr_entry = find_entry(dir_blocks[0][0], curr_dir); //entry of root dir EXT2_ROOT_INO 
  int exact_block;
  while (curr_entry != NULL && curr_dir != NULL) { //end of dest or not root entry & .
      found = 1; 
      // find next successive level directory name
      curr_dir = strtok(NULL, slash); // if root => null 
      parent_block = block_of_dir_inode(curr_entry->inode);
      for (i=0; i < 15 && parent_block[i] != 0; i++) {
        exact_block = parent_block[i];
        if (curr_dir!= NULL) {//if not reach the end of dest
          // reset flag
          found = 0;
          curr_entry = find_entry(exact_block, curr_dir);  //find next directory entry
        }
      }  
  }
  printf("found %d\n", found);
 // store the last obtained entry
  // int* found_block;
  // int correct_block;
   struct ext2_dir_entry * parent_entry = curr_entry;
    if (found == 1) {
      if (curr_dir == NULL) { 
        // if reach the end of all entry in file tree
         strncpy(dir_name, basename(dest), EXT2_NAME_LEN); 
        if (file_type == EXT2_FT_REG_FILE) { 
          // copying file
          printf("hi\n");
          printf("parent_entry %d\n", parent_entry->name_len);
          if (parent_entry->file_type == EXT2_FT_DIR) {
            // if last found entry is a directory
             printf("copying file: dirname: %s\n", dir_name);
             // find if entry with dest name already exists
             // found_block = block_of_dir_inode(parent_entry->inode);
             // for (i=0; i < 15 && found_block[i] != 0; i++) {
             //    correct_block = found_block[i];
             //  }  
             curr_entry = find_entry(exact_block, dir_name);
             // want to copy into this dir
            if (curr_entry) { 
              // exists

              if (curr_entry->file_type == file_type) {
                // exists as a file
                return curr_entry;
              }
              // exists as a directory
              parent_block = block_of_dir_inode(curr_entry->inode);
              // check if file with src name exists in this directory
              struct ext2_dir_entry *child_entry;
              for (i=0; i < 15 && parent_block[i] != 0; i++) {
                exact_block = parent_block[i];
                child_entry = find_entry(exact_block, basename(src));

                if (!child_entry){
                // if not, return this dir 
                   return curr_entry; 
                }
              }
              return child_entry;
            }
            // entry with dest name DNE in last level
            else { 

            //find if exists as a file with given dir_name (basename)
             // struct ext2_dir_entry *child_entry = find_entry(exact_block, dir_name); 
            //  if (child_entry->file_type != file_type) {
            //   fprintf(stderr, "name occupied by other type\n");
            //   exit(-ENOENT);
            //  }
            //  if (!child_entry){
              printf("parent %d\n", parent_entry->file_type);
              return parent_entry; // return its parent
              printf("kkk");
             // } 
            //  else {
            //  return child_entry;
            //  }
            }
          }
        }
        if (file_type == EXT2_FT_DIR) {
         // finding dir
         // printf("making dir: dirname: %s\n", dir_name);
          curr_entry = find_entry(exact_block, dir_name); //finding any with given dir_name (basename)
          if (curr_entry) { //whether basename exists as a dir
            if (curr_entry->file_type != file_type) {
              fprintf(stderr, "Name occupied by other type\n");
              exit(-ENOTDIR);
             }
             return curr_entry;
          }
          // else dir with same name DNE so
          // return parent dir
           return parent_entry;
        }
      }
      // haven't reach the end of all entry but found the required entry
      else {
        return curr_entry;
      }
            
    }
    // not found the required dir entry
    else {
      fprintf(stderr, "Invalid path!\n"); //error exit 
      exit(-ENOENT);
    }
    free(path);
    return NULL;
}
void update_link_count(int block_num) {
    struct ext2_dir_entry * curr_entry= find_entry(block_num, "."); // find parent entry
    // printf("parent_inode %d\n", curr_entry->inode);
    inodes[curr_entry->inode-1].i_links_count+= 1;
}
      
int next_free_inode() {
  if (group->bg_free_inodes_count == 0) {
    return -1;
  }
  for (int i = 0; i < 32; i++) {
    if(inode_bitmap[i]==0) {
      return i+1;
    }
  }

  return -1;
}

int next_free_block() {
  int last;
  if (group->bg_free_blocks_count == 0) {
    return -1;
  }
  for (int j = 0; j < 126; j++) {
    if (block_bitmap[j]) {
      last = j;
    }
  }
  printf("free block %d\n", last);
  return last+2;
}

int update_remove(int block_num, char* entry_name) {
  printf("block_num %d\n", block_num);
  struct ext2_dir_entry *entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * block_num); //.
  struct ext2_dir_entry *previous_entry;
  int offset = 0;
  int rec_len = 0;
  int prev_len = 0;
  int curr_len = 0;
  
  // int found = 0;
  // int flag = 0;
  // int update = 0;
  // printf("calculate\n");
  while (offset != EXT2_BLOCK_SIZE) {
    printf("curr %d\n", curr_len); // 0 0 0 0 12 
    // previous_entry = entry; // .
    prev_len = entry->rec_len; // 12 12 12 20 980
    entry = (struct ext2_dir_entry *)((char*)entry + rec_len); //+0(.) +12(..) +24(lost+found) +44(r) +980
    int comp_size = compare_len(strlen(entry_name), entry->name_len);
    // printf("rec %d\n", rec_len); // 0 12 12 20 12 
    offset += rec_len; // 0 12 24 44
    if (entry->name_len > comp_size) {
        comp_size = entry->name_len;
    }
    printf("input name %s\n", entry_name);
    printf("compare with %.*s\n", entry->name_len, entry->name);
    if (strncmp(entry->name, entry_name, comp_size) == 0) { // N N N Y  
        curr_len = entry->rec_len; // 980
        previous_entry = (struct ext2_dir_entry *)((char*)entry - prev_len); 
        previous_entry->rec_len += curr_len;
        // go to the next entry
        // entry = (struct ext2_dir_entry *)((char*)entry + curr_len); 
        //special cases:

        if ((offset + curr_len) == EXT2_BLOCK_SIZE) { // Y
          // this entry is the final entry
          // go back to its previous
          // offset += rec_len;
          // previous_entry = (struct ext2_dir_entry *)((char*)entry - prev_len); //
          // if (strcmp(previous_entry->name, "..") || strcmp(previous_entry->name, "lost+found")) { //  Y
          // // deleting the only non-dir entry
          // // free the whole block
          //   if (block_num != dir_blocks[0][0]) { //N
          //     block_bitmap[block_num] = 0;
          //     block_bits[block_num/8] |= (0 << (block_num % 8));
          //     entry->inode = 0;
          //   }
          // }
          // end the loop
          printf("end\n");
          offset += curr_len;
          // previous_entry = (struct ext2_dir_entry *)((char*)entry - prev_len); 
          // previous_entry->rec_len += curr_len; //
          // update = 1;
          // skip to the next entry
          // offset += curr_len;
        }
      }
      rec_len = entry->rec_len; //12 12 20 980
      printf("rec %d\n", rec_len);
      
    }
  return curr_len;  
}


//search the parent dir of file with find_entry
//go though the dir
//check rec_len 
//if entry->rec_len > rec_len_needed
// deleted_entry = entry + rec_len_needed .
struct ext2_dir_entry *find_deleted_file(int block_num, char* dir_name) {
    struct ext2_dir_entry *entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * block_num);
    struct ext2_dir_entry *deleted_entry;
    int offset = EXT2_BLOCK_SIZE;
    int rec_len_need; 
    char entry_name[EXT2_NAME_LEN];
    printf("find_deleted_file\n");
    while (offset > 0) {
      rec_len_need = sizeof(struct ext2_dir_entry) + entry->name_len-(sizeof(struct ext2_dir_entry) + entry->name_len)%4+4;
      if (entry->rec_len > rec_len_need) {
        deleted_entry = (struct ext2_dir_entry *)((char*)entry + rec_len_need);
        strncpy(entry_name, deleted_entry->name, deleted_entry->name_len);
        int comp_len = compare_len(strlen(dir_name), deleted_entry->name_len);
        printf("length %d\n", deleted_entry->name_len);
        if (deleted_entry) {
          printf("seg\n");
          printf("compare input %s with %s\n", dir_name, entry_name);
          if (strncmp(entry_name, dir_name, comp_len) == 0) {
            if (deleted_entry->file_type == EXT2_FT_DIR) {
              fprintf(stderr, "Not a file \n");
              exit(-EISDIR);
            }
            return deleted_entry;
          }
          printf("out\n");
        }
      }
      offset = offset - entry->rec_len;
      entry = (struct ext2_dir_entry *)((char*)entry + entry->rec_len);
    }
    return NULL;
}

/*
struct ext2_dir_entry * find_existing_entry(char* path) {
  if (path[0] != '/') {
    printf("not absolute\n");
    fprintf(stderr, "Invalid path\n");
    exit(ENOENT);
  } 
  struct ext2_dir_entry *entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * block_of_dir_inode(EXT2_ROOT_INO));
  if (!strcmp(path, "/"))  {
    return entry;
  } 

  const char slash[2] = "/";
  char *curr_dir = strtok(path, slash);
   int found;

   while (curr_dir) { //not end of path
     printf("curr_dir: %s\n", curr_dir);
     found = 0;
     int offset = EXT2_BLOCK_SIZE;
     while (offset != 0) { //find a name in a dir
        printf("entry name: %s %d\n", entry->name, entry->name_len);
        if (strncmp(entry->name, curr_dir, entry->name_len) == 0) {
          found = 1;
          break;
      }
      offset = offset - entry->rec_len;
      entry = (struct ext2_dir_entry *)((char*)entry + entry->rec_len);
    }

    curr_dir = strtok(NULL, slash); //next entry name //null
    if (found == 1) { //found the entry name
      printf("herrrrrrrrrrrrrrrrrrreeee\n");
      if (!curr_dir) {  //end of path
        return entry;
      } else { 
        if (entry->file_type != EXT2_FT_DIR) { //if the entry is not a dir
          fprintf(stderr, "Invalid path");
          exit(-ENOENT);
        }
        entry = (struct ext2_dir_entry *)(disk + EXT2_BLOCK_SIZE * block_of_dir_inode(entry->inode)); //a
      }
    } else {
      return NULL;
    }
  }
  return NULL;
}*/

