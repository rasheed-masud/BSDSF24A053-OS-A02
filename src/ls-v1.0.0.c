/*
 * ls-v1.1.0 : adds the -l (long listing) option
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

void do_ls(const char *dir);
void do_ls_long(const char *dir);
void mode_to_string(mode_t mode, char *str);
void print_long_entry(const char *dir, const char *name,
                      int lw, int uw, int gw, int sw);

int main(int argc, char *argv[])
{
    int opt, long_flag = 0;

    while ((opt = getopt(argc, argv, "l")) != -1) {
        switch (opt) {
        case 'l':
            long_flag = 1;
            break;
        default:
            fprintf(stderr, "Usage: %s [-l] [directory...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    int ndirs = argc - optind;
    if (ndirs == 0) {
        long_flag ? do_ls_long(".") : do_ls(".");
    } else {
        for (int i = optind; i < argc; i++) {
            if (ndirs > 1)
                printf("%s:\n", argv[i]);
            long_flag ? do_ls_long(argv[i]) : do_ls(argv[i]);
            if (ndirs > 1 && i < argc - 1)
                printf("\n");
        }
    }
    return 0;
}

/* original behaviour: one name per line */
void do_ls(const char *dir)
{
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "ls: cannot access '%s': %s\n", dir, strerror(errno));
        return;
    }
    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;
        printf("%s\n", entry->d_name);
    }
    closedir(dp);
}

/* st_mode -> "drwxr-xr-x" (11 bytes needed in str) */
void mode_to_string(mode_t m, char *s)
{
    if      (S_ISREG(m))  s[0] = '-';
    else if (S_ISDIR(m))  s[0] = 'd';
    else if (S_ISLNK(m))  s[0] = 'l';
    else if (S_ISCHR(m))  s[0] = 'c';
    else if (S_ISBLK(m))  s[0] = 'b';
    else if (S_ISFIFO(m)) s[0] = 'p';
    else if (S_ISSOCK(m)) s[0] = 's';
    else                  s[0] = '?';

    s[1] = (m & S_IRUSR) ? 'r' : '-';
    s[2] = (m & S_IWUSR) ? 'w' : '-';
    s[3] = (m & S_ISUID) ? ((m & S_IXUSR) ? 's' : 'S')
                         : ((m & S_IXUSR) ? 'x' : '-');

    s[4] = (m & S_IRGRP) ? 'r' : '-';
    s[5] = (m & S_IWGRP) ? 'w' : '-';
    s[6] = (m & S_ISGID) ? ((m & S_IXGRP) ? 's' : 'S')
                         : ((m & S_IXGRP) ? 'x' : '-');

    s[7] = (m & S_IROTH) ? 'r' : '-';
    s[8] = (m & S_IWOTH) ? 'w' : '-';
    s[9] = (m & S_ISVTX) ? ((m & S_IXOTH) ? 't' : 'T')
                         : ((m & S_IXOTH) ? 'x' : '-');
    s[10] = '\0';
}

static void owner_name(uid_t uid, char *buf, size_t n)
{
    struct passwd *pw = getpwuid(uid);
    if (pw) snprintf(buf, n, "%s", pw->pw_name);
    else    snprintf(buf, n, "%u", (unsigned)uid);
}

static void group_name(gid_t gid, char *buf, size_t n)
{
    struct group *gr = getgrgid(gid);
    if (gr) snprintf(buf, n, "%s", gr->gr_name);
    else    snprintf(buf, n, "%u", (unsigned)gid);
}

void print_long_entry(const char *dir, const char *name,
                      int lw, int uw, int gw, int sw)
{
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "%s/%s", dir, name);

    struct stat st;
    if (lstat(path, &st) == -1) {        /* lstat: do not follow symlinks */
        perror(path);
        return;
    }

    char perms[11], user[64], grp[64];
    mode_to_string(st.st_mode, perms);
    owner_name(st.st_uid, user, sizeof(user));
    group_name(st.st_gid, grp, sizeof(grp));

    /* ctime() gives "Wed Jun 30 21:49:08 1993\n": slice out the pieces */
    char *t = ctime(&st.st_mtime);
    time_t now = time(NULL);
    int recent = (st.st_mtime <= now) && (now - st.st_mtime < 15778476); /* ~6 months */

    printf("%s %*lu %-*s %-*s %*lld %.3s %.2s ",
           perms, lw, (unsigned long)st.st_nlink,
           uw, user, gw, grp, sw, (long long)st.st_size,
           t + 4, t + 8);
    if (recent) printf("%.5s", t + 11);   /* hh:mm */
    else        printf("%5.4s", t + 20);  /* year, right aligned */
    printf(" %s", name);

    if (S_ISLNK(st.st_mode)) {
        char target[PATH_MAX];
        ssize_t len = readlink(path, target, sizeof(target) - 1);
        if (len != -1) {
            target[len] = '\0';
            printf(" -> %s", target);
        }
    }
    printf("\n");
}

void do_ls_long(const char *dir)
{
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "ls: cannot access '%s': %s\n", dir, strerror(errno));
        return;
    }

    /* pass 1: compute column widths and the "total" line */
    int lw = 1, uw = 1, gw = 1, sw = 1;
    long long blocks = 0;
    struct dirent *entry;
    char path[PATH_MAX], buf[64], num[32];
    struct stat st;

    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;
        snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
        if (lstat(path, &st) == -1)
            continue;
        blocks += st.st_blocks;                       /* 512-byte units */
        int l;
        l = snprintf(num, sizeof(num), "%lu", (unsigned long)st.st_nlink);
        if (l > lw) lw = l;
        owner_name(st.st_uid, buf, sizeof(buf));
        l = (int)strlen(buf); if (l > uw) uw = l;
        group_name(st.st_gid, buf, sizeof(buf));
        l = (int)strlen(buf); if (l > gw) gw = l;
        l = snprintf(num, sizeof(num), "%lld", (long long)st.st_size);
        if (l > sw) sw = l;
    }
    printf("total %lld\n", (blocks + 1) / 2);         /* ls shows 1K blocks */

    /* pass 2: print */
    rewinddir(dp);
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;
        print_long_entry(dir, entry->d_name, lw, uw, gw, sw);
    }
    closedir(dp);
}
