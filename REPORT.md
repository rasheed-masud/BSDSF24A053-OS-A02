# REPORT.md - Operating Systems Programming Assignment 02 (ls utility)

Answers to the report questions for every feature.

## Feature 2 - ls-v1.1.0: Long Listing Format

**Q1. What is the crucial difference between `stat()` and `lstat()`? When is `lstat()` more appropriate for `ls`?**

`stat()` follows symbolic links: if the path is a symlink, it returns information about the file the link *points to*. `lstat()` does not follow the link; it returns information about the link itself (its type is `S_IFLNK`, its size is the length of the target path, and so on). `ls -l` has to use `lstat()` because it must show a symlink as a symlink (`l` in the first column, then `name -> target`). With `stat()` the link would look like the target file, and a dangling (broken) link would make `stat()` fail with `ENOENT` even though the link itself exists and should be listed. `lstat()` is also what prevents a recursive `ls -R` from following a link to a directory and looping forever.

**Q2. How can bitwise operators and macros extract the file type and permissions from `st_mode`?**

`st_mode` packs the file type into the high bits and the permission bits into the low 12 bits. To read one piece, AND the field with a mask and compare: `(st_mode & S_IFMT) == S_IFDIR` tells us the file is a directory (`S_IFMT` is the mask for all the type bits). The convenience macros `S_ISDIR(m)`, `S_ISREG(m)`, `S_ISLNK(m)` and so on do exactly that. For permissions, each bit is tested separately: `st_mode & S_IRUSR` is non-zero if the owner can read, `S_IWUSR` write, `S_IXUSR` execute, with `S_IRGRP/S_IWGRP/S_IXGRP` for the group and `S_IROTH/S_IWOTH/S_IXOTH` for others. My `mode_to_string()` turns each test into `r`, `w`, `x` or `-`. The special bits `S_ISUID`, `S_ISGID` and `S_ISVTX` change the execute position to `s`/`S` (setuid, setgid) or `t`/`T` (sticky): lowercase when the execute bit is also set, uppercase when it is not.
