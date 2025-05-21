#ifndef FS_STRUCTS_H
#define FS_STRUCTS_H

#include <sys/types.h>
#include <time.h>

#define MAX_NAME_LEN 255
#define MAX_CHILDREN 100
#define BLOCK_SIZE 4096
#define MAX_BLOCKS 1024

typedef enum {
    FILETYPE_FILE,
    FILETYPE_DIR
} file_type_t;

typedef struct file_entry {
    char name[MAX_NAME_LEN];
    file_type_t type;
    mode_t permissions;
    uid_t owner;
    gid_t group;
    size_t size;
    time_t created_at;
    time_t modified_at;
    time_t accessed_at;

    
    struct file_entry *children[MAX_CHILDREN];
    int child_count;

   
    char *data_blocks[MAX_BLOCKS];
    int block_count;

    struct file_entry *parent;
} file_entry_t;


extern file_entry_t *root;

void init_file_system();

#endif
