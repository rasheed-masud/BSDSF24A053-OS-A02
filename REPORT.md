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
