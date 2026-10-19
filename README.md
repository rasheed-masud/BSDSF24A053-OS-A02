# BSDSF24A053-OS-A02

<div align="center">

![C](https://img.shields.io/badge/Language-C-blue.svg)
![Linux](https://img.shields.io/badge/Platform-Linux%20%2F%20Unix-lightgrey.svg)
![Assignment](https://img.shields.io/badge/Type-Operating%20Systems%20Assignment-orange.svg)

A custom implementation of the Unix `ls` command written in C for the Operating Systems assignment.

</div>

## Overview

This project recreates a simplified version of the standard Unix `ls` command to list directory contents, inspect file metadata, sort entries, and support recursive directory traversal. The implementation is lightweight, portable, and built using standard C libraries with Linux/Unix system calls such as `opendir()`, `readdir()`, `stat()`, and `lstat()`.

It is designed to demonstrate core operating system concepts including:

- directory traversal
- file and directory metadata handling
- permission and type inspection
- terminal-aware formatting
- recursive listing behavior

## Features

- Default directory listing
- Long format output with `-l`
- Recursive listing with `-R`
- Horizontal column layout with `-x`
- Alphabetical sorting of entries
- File type and permission inspection using `stat`/`lstat`
- Colored output for executables, directories, and special files
- Terminal-aware formatting for different window widths

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
└── LICENSE (if present)
```

### Key Files

- `src/ls-v1.0.0.c` — main implementation of the `ls` utility
- `Makefile` — build automation for compiling the project
- `bin/ls` — generated executable output
- `obj/` — object files from compilation
- `REPORT.md` — project report or documentation
- `man/` — optional manual/support assets

## Requirements

Before building the project, make sure the following are installed:

- GCC or any compatible C compiler
- Make
- Linux or a Unix-like environment

## Build

From the project root, run:

```bash
make
```

This compiles the source and creates the executable in `bin/`:

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
- `--help` — usage help (if implemented)

## Examples

```bash
# Basic listing
./bin/ls

# Long format listing
./bin/ls -l

# Recursive directory listing
./bin/ls -R src/

# Horizontal column output
./bin/ls -x

# List a specific directory
./bin/ls /etc

# Recursive long listing
./bin/ls -l -R .
```

## Clean Build Artifacts

To remove compiled objects and generated binaries:

```bash
make clean
```

This removes generated files from `bin/` and `obj/` while keeping the source code intact.

## Notes

This project is intended for educational and academic purposes. It demonstrates how a simplified Unix-style file listing utility can be implemented from scratch using system calls and core C programming concepts.

## Author

- Student ID: `BSDSF24A053`
- Course: `Operating Systems Assignment 02`

## License

This project is provided for academic and learning purposes.

---

<p align="center">
  <strong>Built as a practical Unix command-line simulation in C.</strong>
</p>
