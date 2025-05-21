#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fs_structs.h"

#define MAX_CMD_LEN 512

void start_shell() {
    char command[MAX_CMD_LEN];

    while (1) {
        printf("myfs> ");
        fflush(stdout);

        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }

        // Remove newline character
        command[strcspn(command, "\n")] = 0;

        if (strcmp(command, "exit") == 0) {
            printf("Exiting myfs...\n");
            break;
        }

        // Process the command
        process_command(command);
    }
}
void myfs_ls(const char *path) {
    file_entry_t *dir = find_entry(path);

    if (!dir || dir->type != FILETYPE_DIRECTORY) {
        printf("Directory not found: %s\n", path);
        return;
    }

    printf("Listing of %s:\n", path);

    for (int i = 0; i < dir->child_count; i++) {
        file_entry_t *child = dir->children[i];
        printf("  %s\t(%s)\n", child->name, 
               (child->type == FILETYPE_DIRECTORY) ? "dir" : "file");
    }
}

void process_command(const char *cmd) {
    char op[10], arg1[MAX_NAME_LEN * 2], arg2[MAX_NAME_LEN * 2];

    if (sscanf(cmd, "%s %s %[^\n]", op, arg1, arg2) < 1) {
        printf("Invalid command!\n");
        return;
    }

    if (strcmp(op, "mkdir") == 0) {
        if (myfs_mkdir(arg1, 0755) == 0)
            printf("Directory created: %s\n", arg1);
        else
            printf("Failed to create directory: %s\n", arg1);
    } else if (strcmp(op, "touch") == 0) {
        if (myfs_mknod(arg1, 0644, 0) == 0)
            printf("File created: %s\n", arg1);
        else
            printf("Failed to create file: %s\n", arg1);
    } else if (strcmp(op, "rm") == 0) {
        if (myfs_unlink(arg1) == 0)
            printf("File deleted: %s\n", arg1);
        else
            printf("Failed to delete file: %s\n", arg1);
    } else if (strcmp(op, "rmdir") == 0) {
        if (myfs_rmdir(arg1) == 0)
            printf("Directory deleted: %s\n", arg1);
        else
            printf("Failed to delete directory: %s\n", arg1);
    }else if (strcmp(op, "ls") == 0) {
    myfs_ls(arg1);
}
 else {
        printf("Unknown command: %s\n", op);
    }
}
