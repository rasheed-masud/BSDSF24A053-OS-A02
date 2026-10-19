/*
 * ls-v1.5.0 : colorized output by file type
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <limits.h>

#define SPACING 2

typedef enum { MODE_DEFAULT, MODE_LONG, MODE_HORIZONTAL } display_t;

static display_t display_mode = MODE_DEFAULT;

void do_ls(const char *dir);
char **read_names(const char *dir, int *count, int *maxlen);
void free_names(char **names, int n);
int  get_term_width(void);
void display_columns(const char *dir, char **names, int n, int maxlen);
void display_horizontal(const char *dir, char **names, int n, int maxlen);
void display_long(const char *dir, char **names, int n);
void mode_to_string(mode_t mode, char *str);
void print_long_entry(const char *dir, const char *name,
                      int lw, int uw, int gw, int sw);
int  compare_names(const void *a, const void *b);
void print_colored(const char *name, mode_t mode);
void print_name(const char *dir, const char *name);

int main(int argc, char *argv[])
{
    int opt;

    while ((opt = getopt(argc, argv, "lx")) != -1) {
        switch (opt) {
        case 'l':
            display_mode = MODE_LONG;
            break;
        case 'x':
            display_mode = MODE_HORIZONTAL;
            break;
        default:
            fprintf(stderr, "Usage: %s [-lx] [directory...]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    int ndirs = argc - optind;
    if (ndirs == 0) {
        do_ls(".");
    } else {
        for (int i = optind; i < argc; i++) {
            if (i > optind)
                printf("\n");
            if (ndirs > 1)
                printf("%s:\n", argv[i]);
            do_ls(argv[i]);
        }
    }
    return 0;
}

void do_ls(const char *dir)
{
    int n, maxlen;
    char **names = read_names(dir, &n, &maxlen);
    if (names == NULL)
        return;

    qsort(names, n, sizeof(char *), compare_names);   /* sort before ANY display */

    switch (display_mode) {
    case MODE_LONG:
        display_long(dir, names, n);
        break;
    case MODE_HORIZONTAL:
        display_horizontal(dir, names, n, maxlen);
        break;
    default:
        display_columns(dir, names, n, maxlen);
        break;
    }

    free_names(names, n);
}

/* Read every visible entry into a dynamic array of strings.
 * Returns NULL on error. *count = number of names, *maxlen = longest name. */
char **read_names(const char *dir, int *count, int *maxlen)
{
    DIR *dp = opendir(dir);
    if (dp == NULL) {
        fprintf(stderr, "ls: cannot access '%s': %s\n", dir, strerror(errno));
        return NULL;
    }
    int cap = 64, n = 0, max = 0;
    char **names = malloc(cap * sizeof(char *));
    if (names == NULL) { perror("malloc"); exit(EXIT_FAILURE); }

    struct dirent *entry;
    while ((entry = readdir(dp)) != NULL) {
        if (entry->d_name[0] == '.')
            continue;
        if (n == cap) {                         /* grow the array */
            cap *= 2;
            char **tmp = realloc(names, cap * sizeof(char *));
            if (tmp == NULL) { perror("realloc"); exit(EXIT_FAILURE); }
            names = tmp;
        }
        names[n] = strdup(entry->d_name);
        if (names[n] == NULL) { perror("strdup"); exit(EXIT_FAILURE); }
        int len = (int)strlen(names[n]);
        if (len > max) max = len;
        n++;
    }
    closedir(dp);
    *count = n;
    *maxlen = max;
    return names;
}

void free_names(char **names, int n)
{
    for (int i = 0; i < n; i++)
        free(names[i]);
    free(names);
}

int get_term_width(void)
{
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0 && ws.ws_col > 0)
        return ws.ws_col;
    return 80;                                   /* fallback */
}

/* "down then across": item index = row + col * rows */
void display_columns(const char *dir, char **names, int n, int maxlen)
{
    if (n == 0) return;
    int colw  = maxlen + SPACING;
    int width = get_term_width();
    int cols  = width / colw;
    if (cols < 1) cols = 1;
    if (cols > n) cols = n;
    int rows  = (n + cols - 1) / cols;           /* ceil(n / cols) */

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < cols; c++) {
            int idx = r + c * rows;
            if (idx >= n) break;
            int len = (int)strlen(names[idx]);
            print_name(dir, names[idx]);
            if (idx + rows < n)                  /* another item follows on this row */
                printf("%*s", colw - len, "");
        }
        printf("\n");
    }
}

/* "across": fill the line left to right, wrap when the next column won't fit */
void display_horizontal(const char *dir, char **names, int n, int maxlen)
{
    if (n == 0) return;
    int colw  = maxlen + SPACING;
    int width = get_term_width();
    int pos = 0, prev = 0;

    for (int i = 0; i < n; i++) {
        int len = (int)strlen(names[i]);
        if (i > 0) {
            if (pos + colw > width) {            /* would overflow: new line */
                printf("\n");
                pos = 0;
            } else {
                printf("%*s", colw - prev, "");  /* pad previous item */
            }
        }
        print_name(dir, names[i]);
        pos += colw;
        prev = len;
    }
    printf("\n");
}


/* qsort passes POINTERS TO the array elements; each element is a char *,
 * so each argument is really a char ** hidden behind const void *. */
int compare_names(const void *a, const void *b)
{
    const char *s1 = *(char * const *)a;
    const char *s2 = *(char * const *)b;
    return strcmp(s1, s2);
}


/* ---- colors (ANSI escape codes) ---- */
#define COLOR_RESET   "\033[0m"
#define COLOR_BLUE    "\033[0;34m"
#define COLOR_GREEN   "\033[0;32m"
#define COLOR_RED     "\033[0;31m"
#define COLOR_PINK    "\033[0;35m"
#define COLOR_REVERSE "\033[7m"

static int has_suffix(const char *name, const char *suffix)
{
    size_t ln = strlen(name), ls = strlen(suffix);
    return ln > ls && strcmp(name + ln - ls, suffix) == 0;
}

/* print a file name in the colour that matches its type */
void print_colored(const char *name, mode_t mode)
{
    const char *color = NULL;

    if (S_ISLNK(mode))
        color = COLOR_PINK;
    else if (S_ISDIR(mode))
        color = COLOR_BLUE;
    else if (S_ISCHR(mode) || S_ISBLK(mode) || S_ISFIFO(mode) || S_ISSOCK(mode))
        color = COLOR_REVERSE;
    else if (has_suffix(name, ".tar") || has_suffix(name, ".gz") ||
             has_suffix(name, ".zip") || has_suffix(name, ".tgz"))
        color = COLOR_RED;
    else if (mode & (S_IXUSR | S_IXGRP | S_IXOTH))
        color = COLOR_GREEN;

    /* like "ls --color=auto": only emit escape codes when writing to a terminal */
    if (color && isatty(STDOUT_FILENO))
        printf("%s%s%s", color, name, COLOR_RESET);
    else
        printf("%s", name);
}

/* stat the entry (lstat: links are not followed) and print it coloured */
void print_name(const char *dir, const char *name)
{
    char path[PATH_MAX];
    struct stat st;
    snprintf(path, sizeof(path), "%s/%s", dir, name);
    if (lstat(path, &st) == -1)
        printf("%s", name);
    else
        print_colored(name, st.st_mode);
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
    printf(" ");
    print_colored(name, st.st_mode);

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


/* long listing over an already-read array of names */
void display_long(const char *dir, char **names, int n)
{
    int lw = 1, uw = 1, gw = 1, sw = 1;
    long long blocks = 0;
    char path[PATH_MAX], buf[64], num[32];
    struct stat st;

    /* pass 1: column widths + "total" line */
    for (int i = 0; i < n; i++) {
        snprintf(path, sizeof(path), "%s/%s", dir, names[i]);
        if (lstat(path, &st) == -1)
            continue;
        blocks += st.st_blocks;                       /* 512-byte units */
        int l = snprintf(num, sizeof(num), "%lu", (unsigned long)st.st_nlink);
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
    for (int i = 0; i < n; i++)
        print_long_entry(dir, names[i], lw, uw, gw, sw);
}
