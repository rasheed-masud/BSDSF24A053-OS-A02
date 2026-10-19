# BSDSF24A053-OS-A02

<div align="center">

<img src="https://img.shields.io/badge/Language-C-blue?style=for-the-badge&logo=c" alt="Language: C" />
<img src="https://img.shields.io/badge/Platform-Linux%20%2F%20Unix-181717?style=for-the-badge&logo=linux" alt="Platform: Linux/Unix" />
<img src="https://img.shields.io/badge/Type-Operating%20Systems%20Assignment-orange?style=for-the-badge" alt="Type: Operating Systems Assignment" />
<img src="https://img.shields.io/badge/Status-Completed-success?style=for-the-badge" alt="Status: Completed" />

</div>

<div align="center">

# Unix-like `ls` Implementation in C

A custom file listing utility inspired by the Unix `ls` command, built as part of the Operating Systems assignment.

</div>

## Overview

This project implements a simplified but functional Unix-style `ls` command in C. It supports listing directory contents, displaying long-form metadata, sorting entries alphabetically, handling recursive traversal, and formatting output based on terminal width.

The utility demonstrates several core operating system principles, including:

- directory traversal using `opendir()` and `readdir()`
- file metadata access with `stat()` and `lstat()`
- permission and file type inspection
- recursive directory walking
- terminal-aware layout calculations
- colored output for better terminal readability

## Features

- Default directory listing
- Long format output with `-l`
- Recursive listing with `-R`
- Horizontal column layout with `-x`
- Alphabetical sorting of entries
- File type and permission detection
- Executable and directory highlighting in color
- Terminal-width aware formatting

## Demo Output

```bash
$ ./bin/ls
bin  Makefile  obj  REPORT.md  src  README.md

$ ./bin/ls -l
total 48
drwxr-xr-x  4 rasheed  staff  4096  Oct 19 12:42 bin
drwxr-xr-x  2 rasheed  staff  4096  Oct 19 12:42 obj
-rw-r--r--  1 rasheed  staff  2048  Oct 19 12:42 Makefile
-rw-r--r--  1 rasheed  staff  1024  Oct 19 12:42 README.md
-rw-r--r--  1 rasheed  staff  4096  Oct 19 12:42 REPORT.md
-rw-r--r--  1 rasheed  staff  4096  Oct 19 12:42 src

$ ./bin/ls -R .
.
./bin
./obj
./src
./src/ls-v1.0.0.c

$ ./bin/ls -x
bin      Makefile   obj      REPORT.md
README.md  src
```

## Project Structure

```text
.
├── Makefile
├── README.md
├── REPORT.md
├── src/
│   └── ls-v1.0.0.c
├── obj/
├── bin/
├── man/
├── .gitignore
└── LICENSE (optional)
```

### Main Files

- `src/ls-v1.0.0.c` — core implementation of the `ls` utility
- `Makefile` — build automation
- `bin/ls` — generated executable
- `obj/` — compiled object files
- `REPORT.md` — assignment report
- `man/` — optional manual or support files

## Requirements

Before building the project, ensure the following are installed:

- GCC or any compatible C compiler
- Make
- Linux or Unix-like environment

## Build

From the root directory, run:

```bash
make
```

This builds the executable in the `bin/` directory:

```bash
./bin/ls
```

## Usage

```bash
./bin/ls [options] [file_or_directory ...]
```

### Available Options

- `-l` — long listing format
- `-R` — recursive listing
- `-x` — horizontal column display
- `--help` — usage information if supported by the implementation

## Examples

```bash
# Basic directory listing
./bin/ls

# Long format listing
./bin/ls -l

# Recursive listing
./bin/ls -R src/

# Horizontal output layout
./bin/ls -x

# List a specific directory
./bin/ls /etc

# Recursive long listing
./bin/ls -l -R .
```

## Clean Build Artifacts

To remove generated binaries and object files:

```bash
make clean
```

This will delete files from `bin/` and `obj/` while keeping the source code intact.

## Notes

This project is intended for educational and academic use. It demonstrates how a simplified Unix-style file listing utility can be implemented from scratch using system calls, terminal control, and standard C programming techniques.

## Author

- Student ID: `BSDSF24A053`
- Course: `Operating Systems Assignment 02`

## License

This project is provided for academic and learning purposes.

---

<div align="center">

<strong>Built as a practical Unix command-line simulation in C.</strong>

</div>
