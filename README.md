# 🧾 Custom `ls` Command Project

## 📘 Overview
This project is a **custom implementation of the UNIX/Linux `ls` command**, developed incrementally across multiple versions.  
Each version adds new functionality or improvements — from basic file listing to advanced features such as color output, recursive traversal, and formatted column display.

The project demonstrates deep understanding of:
- **System calls** (`opendir`, `readdir`, `stat`, `lstat`)
- **File permissions and metadata handling**
- **Formatting and color coding using ANSI escape sequences**
- **Recursive directory traversal**
- **Makefile-based build automation**

---

## 🗂️ Project Directory Structure

```
.
├── bin
│   ├── ls
│   ├── ls-v1.2.0
│   ├── ls-v1.3.0
│   ├── ls-v1.4.0
│   ├── ls-v1.5.0
│   └── ls-v1.6.0
├── Makefile
├── man
├── obj
│   └── ls-v1.0.0.o
│   └── ls-v1.1.0.o
│   └── ls-v1.2.0.o
│   └── ls-v1.3.0.o
│   └── ls-v1.4.0.o
│   └── ls-v1.5.0.o
│   └── ls-v1.6.0.o
├── REPORT.md
└── src
    ├── ls-v1.1.0.c
    ├── ls-v1.2.0.c
    ├── ls-v1.3.0.c
    ├── ls-v1.4.0.c
    ├── ls-v1.5.0.c
    └── ls-v1.6.0.c
```

### Directory Explanation:
| Folder/File | Description |
|--------------|--------------|
| **bin/** | Contains all compiled binaries of different `ls` versions |
| **src/** | Source code of each version (`.c` files) |
| **obj/** | Object files generated during compilation |
| **man/** | (Optional) Directory for manual or documentation files |
| **Makefile** | Automates compilation and cleaning tasks |
| **REPORT.md** | Contains detailed answers and explanations for report questions |
| **README.md** | This file — provides project overview and usage details |

---

## ⚙️ Building the Project

### 1. Compile the Latest Version
Run the following command to compile the most recent version (v1.6.0):
```bash
make ls-v1.6.0
```

### 2. Compile All Versions
To compile every version available:
```bash
make all
```

### 3. Clean Object Files and Binaries
```bash
make clean
```

After compilation, the executable files will appear in the **bin/** directory.

---

## 🚀 Running the Program

### Basic Usage
```bash
./bin/ls-v1.6.0 [options] [directory]
```

### Common Options Implemented
| Option | Description |
|---------|--------------|
| `-l` | Long listing format (shows permissions, owner, size, date) |
| `-a` | Shows all files, including hidden ones |
| `-R` | Recursively lists directories |
| `-x` | Displays files across, rather than down, in columns |
| `--color` | Displays color-coded output based on file type |

Example:
```bash
./bin/ls-v1.6.0 -l --color /home/user
```

---

## 🧩 Version History and Features

| Version | File | Description |
|----------|------|-------------|
| **v1.1.0** | `ls-v1.1.0.c` | Basic implementation of `ls` — lists filenames in the current directory |
| **v1.2.0** | `ls-v1.2.0` | Added long listing (`-l`) support using `stat()` |
| **v1.3.0** | `ls-v1.3.0` | Introduced symbolic link handling using `lstat()` |
| **v1.4.0** | `ls-v1.4.0.c` | Implemented color display using ANSI escape codes |
| **v1.5.0** | `ls-v1.5.0.c` | Added recursive traversal (`-R`) for directories |
| **v1.6.0** | `ls-v1.6.0.c` | Final version with column formatting (`-x`), sorting, and refined output layout |

Each version improves upon the previous one, focusing on modularity, readability, and alignment with the real `ls` command’s behavior.

---

## 📄 REPORT.md Overview

The `REPORT.md` file contains written explanations and answers for:
- System call usage (`stat()`, `lstat()`)
- File permissions and types (`st_mode`)
- Column formatting logic
- Recursive traversal and base cases
- Use of ANSI escape codes for coloring
- Sorting mechanism with `qsort()`

It serves as both a **technical report** and **learning documentation** for understanding low-level file system programming.

---

## 🧠 Key Learnings
- Interaction with **POSIX APIs** and file system structures.
- Using **bitwise operations** to extract permissions and file types.
- Implementing **recursive functions** safely with base cases.
- Applying **ANSI color codes** for user-friendly terminal output.
- Managing multi-version builds via a **Makefile**.

---

## 👨‍💻 Author
**Name:** *Hasan Ali*  
**Course/Module:** *Operating Systems*  
**Institution:** *FCIT*  
**Date:** *October, 2025*

---

## 🏁 Conclusion
This project demonstrate show to build a simplified version of a complex Linux command — `ls` — from scratch.  
It provides hands-on experience in **system calls**, **recursion**, **file management**, and **terminal formatting**, reflecting core principles of Unix-based programming and command-line tool development.

