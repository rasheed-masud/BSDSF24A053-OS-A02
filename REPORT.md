# REPORT.md - Operating Systems Programming Assignment 02 (ls utility)

Answers to the report questions for every feature.

## Feature 2 - ls-v1.1.0: Long Listing Format

**Q1. What is the crucial difference between `stat()` and `lstat()`? When is `lstat()` more appropriate for `ls`?**

`stat()` follows symbolic links: if the path is a symlink, it returns information about the file the link *points to*. `lstat()` does not follow the link; it returns information about the link itself (its type is `S_IFLNK`, its size is the length of the target path, and so on). `ls -l` has to use `lstat()` because it must show a symlink as a symlink (`l` in the first column, then `name -> target`). With `stat()` the link would look like the target file, and a dangling (broken) link would make `stat()` fail with `ENOENT` even though the link itself exists and should be listed. `lstat()` is also what prevents a recursive `ls -R` from following a link to a directory and looping forever.

**Q2. How can bitwise operators and macros extract the file type and permissions from `st_mode`?**

`st_mode` packs the file type into the high bits and the permission bits into the low 12 bits. To read one piece, AND the field with a mask and compare: `(st_mode & S_IFMT) == S_IFDIR` tells us the file is a directory (`S_IFMT` is the mask for all the type bits). The convenience macros `S_ISDIR(m)`, `S_ISREG(m)`, `S_ISLNK(m)` and so on do exactly that. For permissions, each bit is tested separately: `st_mode & S_IRUSR` is non-zero if the owner can read, `S_IWUSR` write, `S_IXUSR` execute, with `S_IRGRP/S_IWGRP/S_IXGRP` for the group and `S_IROTH/S_IWOTH/S_IXOTH` for others. My `mode_to_string()` turns each test into `r`, `w`, `x` or `-`. The special bits `S_ISUID`, `S_ISGID` and `S_ISVTX` change the execute position to `s`/`S` (setuid, setgid) or `t`/`T` (sticky): lowercase when the execute bit is also set, uppercase when it is not.

## Feature 3 - ls-v1.2.0: Column Display (Down Then Across)

**Q1. Explain the logic for "down then across" printing. Why is a single loop through the filenames not enough?**

First all names are read into an array and the longest name is found. The column width is `max_len + 2`; the number of columns is `terminal_width / column_width` (at least 1), and the number of rows is `ceil(n / cols)`. The names are laid out column by column (the first `rows` names go in column 0, the next `rows` names in column 1, and so on), but the terminal prints one *row* at a time. So for row `r` and column `c` the item to print is `names[r + c * rows]`, and I print it with an outer loop over rows and an inner loop over columns. A single loop over the array would print `names[0], names[1], names[2]...` on the same line, which is the "across" order. That ordering puts consecutive names next to each other instead of below each other, so a single loop can only produce row-major output. Printing down-then-across needs to jump `rows` positions in the array between neighbours on a line, which needs the row/column calculation, and therefore needs the total count first.

**Q2. What is the purpose of `ioctl` here? What are the limits of a fixed 80-column fallback?**

`ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws)` asks the terminal driver for the window size (`ws.ws_col` is the number of columns), so the layout matches the real terminal. With only a fixed 80 columns, a wide terminal would waste space with too few columns, and a narrow terminal (or a split pane) would get lines longer than the screen that wrap in the middle of names and break the alignment. It also would not react to the user resizing the window. The 80-column fallback is only used when `ioctl` fails, for example when output is redirected to a file or a pipe.

## Feature 4 - ls-v1.3.0: Horizontal Column Display (-x)

**Q1. Compare the complexity of "down then across" and "across" printing. Which needs more pre-calculation?**

The vertical ("down then across") mode needs more pre-calculation. Before printing anything it must know the number of columns and then the number of rows (`ceil(n / cols)`), because the index of each item depends on `rows` (`r + c * rows`), and it needs a nested loop with an index jump. The horizontal mode only needs the column width; it walks the array once in order, keeps a running screen position, prints each name padded to the column width, and starts a new line when the next item would not fit. It never needs to know the number of rows in advance.

**Q2. How did you manage the display modes (-l, -x, default)?**

I use an `enum` (`MODE_DEFAULT`, `MODE_LONG`, `MODE_HORIZONTAL`) stored in a variable `display_mode`. The `getopt()` loop in `main` sets it when it sees `-l` or `-x` (if both are given, the last one wins). `do_ls()` reads all the names into an array and then uses a `switch (display_mode)` to call `display_long()`, `display_horizontal()` or `display_columns()`. Because the decision is made in one place, after the data has been read, adding a new mode is just a new enum value and a new `case`.

## Feature 5 - ls-v1.4.0: Alphabetical Sort

**Q1. Why must all entries be read into memory before sorting? What are the drawbacks for huge directories?**

`readdir()` returns entries in whatever order the file system stores them, and gives them to us one at a time. A sort needs to compare every entry with the others, so the whole set must be available before the first line can be printed; we cannot know which name comes first until we have seen all of them. For a directory with millions of files this means a lot of memory (one `strdup`'d string plus a pointer per entry), a long delay before any output appears, and the cost of sorting (`O(n log n)` comparisons). Unsorted `ls -f` avoids this by printing entries as it reads them, in constant memory.

**Q2. What is the purpose and signature of the `qsort()` comparison function? Why `const void *`?**

`qsort(base, nmemb, size, compar)` is a generic sort that works on any element type, so it does not know how to compare elements. We supply `int compar(const void *a, const void *b)`, which must return a negative number if `*a` sorts before `*b`, zero if equal, and a positive number if after. `qsort` passes pointers *to the array elements*. Our elements are `char *`, so each argument is really a `char **`; inside the function I cast `a` and `b` to `char * const *`, dereference once to get the strings, and return `strcmp(s1, s2)`. The parameters are `const void *` because `qsort` has to accept any element type (`void *` is the generic pointer type in C) and because a comparison must not modify the data (`const`).

## Feature 6 - ls-v1.5.0: Colorized Output

**Q1. How do ANSI escape codes produce colour? Show the sequence for green text.**

The terminal watches the stream of characters it receives. A sequence starting with the ESC character (octal `\033`) followed by `[` is not displayed; it is a control command. `ESC[` followed by numbers and a final `m` is the "select graphic rendition" command, which sets text attributes: for example `0` resets, `7` is reverse video, `31` is red, `32` is green, `34` is blue, `35` is magenta/pink. Everything printed after the command uses that style until it is reset. Green text:

```c
printf("\033[0;32m%s\033[0m", filename);   /* start green, print text, reset */
```

The final `\033[0m` is important; without it the rest of the terminal output stays green.

**Q2. Which bits of `st_mode` determine whether a file is executable?**

There are three execute bits, one for each class of user: `S_IXUSR` (0100) for the owner, `S_IXGRP` (0010) for the group, and `S_IXOTH` (0001) for everyone else. A file is executable by the owner if `st_mode & S_IXUSR` is non-zero, by the group if `st_mode & S_IXGRP` is non-zero, and by others if `st_mode & S_IXOTH` is non-zero. For colouring I test whether any of the three is set: `st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)`. Directories also have `x` bits (meaning "may enter"), so the directory check is done first and a directory is coloured blue, not green.
