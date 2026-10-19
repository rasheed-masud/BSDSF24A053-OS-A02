# Custom Linux `ls` Utility (OS Assignment 02)

[![C Language](https://img.shields.io/badge/Language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

A robust, modular clone of the standard Linux `ls` utility written in C from scratch. Developed as part of the Operating Systems course curriculum.

* **Student Name:** Rasheed Masud
* **Roll Number:** BSDSF24A053
* **Repository:** BSDSF24A053-OS-A02

---

## Features & Version Roadmap

This utility was developed iteratively across multiple versioned stages:

* **v1.0.0:** Basic directory listing (plain single-column format, skipping hidden files).
* **v1.1.0 (`-l`):** Detailed long listing format including file permissions, links, owner, size, and timestamps.
* **v1.2.0 (Columns):** Dynamic multi-column output calculated automatically based on terminal width.
* **v1.3.0 (`-x`):** Horizontal column-first display ordering.
* **v1.4.0 (Sorting):** Alphabetical entry sorting implemented using standard library sorting algorithms (`qsort`).
* **v1.5.0 (Colors):** ANSI colorized terminal output categorized by file type (directories, executables, regular files).
* **v1.6.0 (`-R`):** Full recursive directory tree traversal.

---

## Project Directory Structure

```text
BSDSF24A053-OS-A02/
├── bin/              # Compiled binary output
├── obj/              # Object files during build
├── src/              # Source code implementation files
├── man/              # Manual documentation
├── Makefile          # Build automation configuration
├── README.md         # Project documentation
└── REPORT.md         # Stage-by-stage engineering reports
