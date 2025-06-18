#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

#define BUFFER_SIZE 1024

void print_help() {
    printf("Available commands:\n");
    printf("  create <filename>\n");
    printf("  delete <filename>\n");
    printf("  mkdir <dirname>\n");
    printf("  rmdir <dirname>\n");
    printf("  ls\n");
    printf("  cat <filename>\n");
    printf("  help\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_help();
        return 1;
    }

    char *cmd = argv[1];
    char *arg1 = argc > 2 ? argv[2] : NULL;

    if (strcmp(cmd, "create") == 0 && arg1) {
        int fd = open(arg1, O_CREAT | O_WRONLY, 0644);
        if (fd < 0)
            perror("create failed");
        else {
            printf("File created: %s\n", arg1);
            close(fd);
        }
    } 
    else if (strcmp(cmd, "delete") == 0 && arg1) {
        if (unlink(arg1) < 0)
            perror("delete failed");
        else
            printf("File deleted: %s\n", arg1);
    }
    else if (strcmp(cmd, "mkdir") == 0 && arg1) {
        if (mkdir(arg1, 0755) < 0)
            perror("mkdir failed");
        else
            printf("Directory created: %s\n", arg1);
    }
    else if (strcmp(cmd, "rmdir") == 0 && arg1) {
        if (rmdir(arg1) < 0)
            perror("rmdir failed");
        else
            printf("Directory removed: %s\n", arg1);
    }
    else if (strcmp(cmd, "ls") == 0) {
        DIR *d = opendir(".");
        if (!d) {
            perror("opendir failed");
            return 1;
        }
        struct dirent *entry;
        while ((entry = readdir(d)) != NULL) {
            if (strcmp(entry->d_name, ".") && strcmp(entry->d_name, ".."))
                printf("%s\n", entry->d_name);
        }
        closedir(d);
    }
    else if (strcmp(cmd, "cat") == 0 && arg1) {
        int fd = open(arg1, O_RDONLY);
        if (fd < 0) {
            perror("cat failed");
            return 1;
        }
        char buf[BUFFER_SIZE];
        int bytes;
        while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
            write(STDOUT_FILENO, buf, bytes);
        }
        close(fd);
    }
    else {
        printf("Invalid command or missing argument.\n");
        print_help();
    }

    return 0;
}
