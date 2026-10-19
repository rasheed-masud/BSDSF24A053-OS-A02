/*
 * ls-v1.0.0 : base version (stand-in for the instructor's starter)
 * Lists the non-hidden entries of each directory, one per line.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>

void do_ls(const char *dir);

int main(int argc, char *argv[])
{
    if (argc == 1) {
        do_ls(".");
    } else {
        for (int i = 1; i < argc; i++) {
            if (argc > 2)
                printf("%s:\n", argv[i]);
            do_ls(argv[i]);
            if (argc > 2 && i < argc - 1)
                printf("\n");
        }
    }
    return 0;
}

void do_ls(const char *dir)
{
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "ls: cannot access '%s': %s\n", dir, strerror(errno));
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')      /* skip hidden files, . and .. */
            continue;
        printf("%s\n", entry->d_name);
    }
    closedir(dp);
}
