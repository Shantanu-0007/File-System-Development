#define FUSE_USE_VERSION 31
#include "fs_structs.h"
#include <fuse3/fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "frontend.c"

static int myfs_getattr(const char *path, struct stat *stbuf, struct fuse_file_info *fi) {
    (void) fi;
    memset(stbuf, 0, sizeof(struct stat));

    if (strcmp(path, "/") == 0) {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    } else {
        return -ENOENT;
    }

    return 0;
}

int main(int argc, char *argv[]) {
	init_file_system();
	printf("Welcome to MyFS Interactive Shell!\n");
    start_shell();
    return fuse_main(argc, argv, &operations, NULL);
}

static struct fuse_operations operations = {
    .getattr = myfs_getattr,
    .readdir = myfs_readdir,
    .mkdir = myfs_mkdir,
    .mknod = myfs_mknod,
    .unlink = myfs_unlink,
    .rmdir = myfs_rmdir,
    .read = myfs_read,
    .write = myfs_write,
};

static int myfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
                        off_t offset, struct fuse_file_info *fi,
                        enum fuse_readdir_flags flags) {
    (void) offset;
    (void) fi;
    (void) flags;

    if (strcmp(path, "/") != 0)
        return -ENOENT;

    filler(buf, ".", NULL, 0, 0);
    filler(buf, "..", NULL, 0, 0);

    return 0;
}

static int myfs_open(const char *path, struct fuse_file_info *fi) {
    return -ENOENT;
}

static struct fuse_operations operations = {
    .getattr = myfs_getattr,
    .readdir = myfs_readdir,
    .open    = myfs_open,
};

