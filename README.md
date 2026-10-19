# BSDSF24A053-OS-A02

A custom implementation of the Unix `ls` command written in C as part of an Operating Systems assignment.

## Overview

This project recreates core `ls` functionality to list directory contents, display metadata, handle sorting, and support recursive traversal. It is built using standard C libraries and compiled with `gcc` via a `Makefile`.

The implementation provides a lightweight command-line utility similar to the standard Unix `ls`, designed to demonstrate essential operating system concepts such as directory traversal, file metadata access, terminal formatting, and recursion.

## Features

- Default directory listing
- Long format output with `-l`
- Recursive listing with `-R`
- Horizontal column layout support with `-x`
- Alphabetical sorting of entries
- File type and permission inspection using `stat`/`lstat`
- Colorized output for executable files and directories
- Terminal-aware column formatting

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
└── .gitignore
```

- `src/ls-v1.0.0.c` — main implementation of the `ls` utility
- `Makefile` — build automation
- `bin/ls` — generated executable
- `obj/` — compiled object files
- `man/` — optional manual-related files or support assets

## Requirements

Before building the project, ensure the following are installed on your system:

- GCC or any C compiler compatible with the GNU C standard
- Make
- Linux or Unix-like environment

## Build the Project

To compile the source code and generate the executable in `bin/`:

```bash
make
```

This creates the binary:

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
- `--help` or similar usage output, if supported by the implementation

## Usage Examples

```bash
# Basic listing
./bin/ls

# Long listing format
./bin/ls -l

# Recursive listing
./bin/ls -R src/

# Horizontal column layout
./bin/ls -x

# List a specific directory
./bin/ls /etc

# Long listing of a directory recursively
./bin/ls -l -R .
```

## Clean Build Artifacts

To remove compiled binaries and object files:

```bash
make clean
```

This deletes generated artifacts from `bin/` and `obj/` while preserving the source files.

## Notes

This project is intended for educational and academic purposes. It demonstrates how a simplified Unix-style file listing utility is implemented using system calls such as `opendir()`, `readdir()`, `stat()`, `lstat()`, and recursive directory traversal.

## Author

- Student ID: BSDSF24A053
- Course: Operating Systems Assignment 02

## License

This project is provided for academic use and learning purposes.
