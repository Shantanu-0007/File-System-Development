#include "fs_structs.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

file_entry_t *root = NULL;

void init_file_system() {
    root = (file_entry_t *)malloc(sizeof(file_entry_t));
    memset(root, 0, sizeof(file_entry_t));
    strcpy(root->name, "/");
    root->type = FILETYPE_DIR;
    root->permissions = 0755;
    root->owner = getuid();
    root->group = getgid();
    root->size = 0;
    root->created_at = root->modified_at = root->accessed_at = time(NULL);
}
typedef struct file_entry {
    char name[MAX_NAME_LEN];
    int type;
    mode_t permissions;
    uid_t owner;
    gid_t group;
    time_t created_at;
    time_t modified_at;
    time_t accessed_at;
    size_t size;
    char content[MAX_FILE_SIZE];
    struct file_entry *parent;
    struct file_entry *children[MAX_CHILDREN];
    int child_count;
} file_entry_t;




file_entry_t* find_entry(const char *path) {
    if (strcmp(path, "/") == 0) return root;

    file_entry_t *current = root;
    char temp_path[MAX_NAME_LEN * 10];
    strcpy(temp_path, path);

    char *token = strtok(temp_path, "/");
    while (token != NULL) {
        int found = 0;
        for (int i = 0; i < current->child_count; i++) {
            if (strcmp(current->children[i]->name, token) == 0) {
                current = current->children[i];
                found = 1;
                break;
            }
        }
        if (!found) return NULL;
        token = strtok(NULL, "/");
    }
    return current;
}

int myfs_mkdir(const char *path, mode_t mode) {
    char parent_path[MAX_NAME_LEN * 10];
    char dirname[MAX_NAME_LEN];

    
    strcpy(parent_path, path);
    char *slash = strrchr(parent_path, '/');
    if (slash == NULL) return -ENOENT;
    
    strcpy(dirname, slash + 1);
    *slash = '\0';
    if (strlen(parent_path) == 0) strcpy(parent_path, "/");

    file_entry_t *parent = find_entry(parent_path);
    if (!parent || parent->type != FILETYPE_DIR) return -ENOENT;
    if (parent->child_count >= MAX_CHILDREN) return -ENOSPC;

    file_entry_t *new_dir = (file_entry_t *)malloc(sizeof(file_entry_t));
    memset(new_dir, 0, sizeof(file_entry_t));
    strcpy(new_dir->name, dirname);
    new_dir->type = FILETYPE_DIR;
    new_dir->permissions = mode;
    new_dir->owner = getuid();
    new_dir->group = getgid();
    new_dir->created_at = new_dir->modified_at = new_dir->accessed_at = time(NULL);
    new_dir->parent = parent;

    parent->children[parent->child_count++] = new_dir;
    parent->modified_at = time(NULL);

    return 0;
}

int myfs_mknod(const char *path, mode_t mode, dev_t rdev) {
    char parent_path[MAX_NAME_LEN * 10];
    char filename[MAX_NAME_LEN];

    strcpy(parent_path, path);
    char *slash = strrchr(parent_path, '/');
    if (slash == NULL) return -ENOENT;

    strcpy(filename, slash + 1);
    *slash = '\0';
    if (strlen(parent_path) == 0) strcpy(parent_path, "/");

    file_entry_t *parent = find_entry(parent_path);
    if (!parent || parent->type != FILETYPE_DIR) return -ENOENT;
    if (parent->child_count >= MAX_CHILDREN) return -ENOSPC;

    file_entry_t *new_file = (file_entry_t *)malloc(sizeof(file_entry_t));
    memset(new_file, 0, sizeof(file_entry_t));
    strcpy(new_file->name, filename);
    new_file->type = FILETYPE_FILE;
    new_file->permissions = mode;
    new_file->owner = getuid();
    new_file->group = getgid();
    new_file->created_at = new_file->modified_at = new_file->accessed_at = time(NULL);
    new_file->parent = parent;

    parent->children[parent->child_count++] = new_file;
    parent->modified_at = time(NULL);

    return 0;
}

int myfs_unlink(const char *path) {
    file_entry_t *file = find_entry(path);
    if (!file || file->type != FILETYPE_FILE) return -ENOENT;

    file_entry_t *parent = file->parent;
    int i;
    for (i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == file) {
            break;
        }
    }

    free(file);
    for (int j = i; j < parent->child_count - 1; j++) {
        parent->children[j] = parent->children[j + 1];
    }
    parent->child_count--;
    parent->modified_at = time(NULL);

    return 0;
}

int myfs_rmdir(const char *path) {
    file_entry_t *dir = find_entry(path);
    if (!dir || dir->type != FILETYPE_DIR) return -ENOENT;
    if (dir->child_count > 0) return -ENOTEMPTY;

    file_entry_t *parent = dir->parent;
    int i;
    for (i = 0; i < parent->child_count; i++) {
        if (parent->children[i] == dir) {
            break;
        }
    }

    free(dir);
    for (int j = i; j < parent->child_count - 1; j++) {
        parent->children[j] = parent->children[j + 1];
    }
    parent->child_count--;
    parent->modified_at = time(NULL);

    return 0;
}

int myfs_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    file_entry_t *file = find_entry(path);
    if (!file || file->type != FILETYPE_FILE)
        return -ENOENT;

    if (offset >= file->size)
        return 0;

    if (offset + size > file->size)
        size = file->size - offset;

    memcpy(buf, file->content + offset, size);
    file->accessed_at = time(NULL);

    return size;
}

int myfs_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
    file_entry_t *file = find_entry(path);
    if (!file || file->type != FILETYPE_FILE)
        return -ENOENT;

    if (offset + size > MAX_FILE_SIZE)
        size = MAX_FILE_SIZE - offset;

    memcpy(file->content + offset, buf, size);
    file->size = offset + size;
    file->modified_at = time(NULL);

    return size;
}


int myfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                 off_t offset, struct fuse_file_info *fi, enum fuse_readdir_flags flags) {

    file_entry_t *dir = find_entry(path);
    if (!dir || dir->type != FILETYPE_DIRECTORY)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    for (int i = 0; i < dir->child_count; i++) {
        file_entry_t *child = dir->children[i];
        filler(buf, child->name, NULL, 0, 0);
    }

    return 0;
}

