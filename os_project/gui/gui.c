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
    printf("  exit\n");
}

int main() {
    char command[BUFFER_SIZE];
    char arg1[BUFFER_SIZE];

    printf("Welcome to FILE-SYSTEM CLI\n");
    print_help();

    while (1) {
        printf("> ");
        fflush(stdout);

        if (fgets(command, sizeof(command), stdin) == NULL)
            break;
        command[strcspn(command, "\n")] = 0;

        if (sscanf(command, "create %s", arg1) == 1) {
            int fd = open(arg1, O_CREAT | O_WRONLY, 0644);
            if (fd < 0)
                perror("create failed");
            else {
                printf("File created: %s\n", arg1);
                close(fd);
            }
        } 
        else if (sscanf(command, "delete %s", arg1) == 1) {
            if (unlink(arg1) < 0)
                perror("delete failed");
            else
                printf("File deleted: %s\n", arg1);
        }
        else if (sscanf(command, "mkdir %s", arg1) == 1) {
            if (mkdir(arg1, 0755) < 0)
                perror("mkdir failed");
            else
                printf("Directory created: %s\n", arg1);
        }
        else if (sscanf(command, "rmdir %s", arg1) == 1) {
            if (rmdir(arg1) < 0)
                perror("rmdir failed");
            else
                printf("Directory removed: %s\n", arg1);
        }
        else if (strncmp(command, "ls", 2) == 0) {
            DIR *d = opendir(".");
            if (!d) {
                perror("opendir failed");
                continue;
            }
            struct dirent *entry;
            while ((entry = readdir(d)) != NULL) {
                printf("%s  ", entry->d_name);
            }
            printf("\n");
            closedir(d);
        }
        else if (sscanf(command, "cat %s", arg1) == 1) {
            int fd = open(arg1, O_RDONLY);
            if (fd < 0) {
                perror("cat failed");
                continue;
            }
            char buf[BUFFER_SIZE];
            int bytes;
            while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
                write(STDOUT_FILENO, buf, bytes);
            }
            printf("\n");
            close(fd);
        }
        else if (strncmp(command, "help", 4) == 0) {
            print_help();
        }
        else if (strncmp(command, "exit", 4) == 0) {
            break;
        }
        else {
            printf("Unknown command. Type 'help' for list of commands.\n");
        }
    }

    printf("Exiting FILE-SYSTEM CLI.\n");
    return 0;
}
