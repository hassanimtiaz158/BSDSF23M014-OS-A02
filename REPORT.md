## **Report Questions**

---

### **Question 1 – Difference between `stat()` and `lstat()`**

Both `stat()` and `lstat()` are system calls used to get information about files, but the **main difference** is how they handle **symbolic links**.

| Function | Description |
|-----------|--------------|
| `stat(path, &buf)` | Returns information about the **file that the path points to**. If it’s a symbolic link, it follows the link and gives details about the **target file**. |
| `lstat(path, &buf)` | Returns information about the **link itself**, not the target file. So, if `path` is a symbolic link, it shows info about the **link** (like its size and permissions). |

#### 💡 Example
If you have a symbolic link `link.txt` pointing to `file.txt`:

- `stat("link.txt", &buf)` → info about `file.txt` (target)  
- `lstat("link.txt", &buf)` → info about `link.txt` (the link)

**In the `ls -l` command:**  
We use **`lstat()`** so that symbolic links are displayed with their own details, not the file they point to.

---

### **Question 2 – Using `st_mode` to Get File Type and Permissions**

The `st_mode` field in the `struct stat` (from `<sys/stat.h>`) stores **two kinds of information**:

1. **File Type** – what kind of file it is (regular file, directory, link, etc.)  
2. **Permissions** – read, write, execute permissions for owner, group, and others

---

### **1️⃣ Checking File Type**

You can find the file type by comparing `(mode & S_IFMT)` with specific macros:

| Macro | Meaning |
|--------|----------|
| `S_IFREG` | Regular file |
| `S_IFDIR` | Directory |
| `S_IFLNK` | Symbolic link |
| `S_IFCHR` | Character device |
| `S_IFBLK` | Block device |
| `S_IFIFO` | FIFO (pipe) |
| `S_IFSOCK` | Socket |

#### **Example Code:**
```c
#include <stdio.h>
#include <sys/stat.h>

void print_file_type(mode_t mode) {
    if (S_ISREG(mode))   printf("Regular file\n");
    else if (S_ISDIR(mode))  printf("Directory\n");
    else if (S_ISLNK(mode))  printf("Symbolic link\n");
    else if (S_ISCHR(mode))  printf("Character device\n");
    else if (S_ISBLK(mode))  printf("Block device\n");
    else if (S_ISFIFO(mode)) printf("FIFO/pipe\n");
    else if (S_ISSOCK(mode)) printf("Socket\n");
    else printf("Unknown type\n");
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    struct stat info;
    if (stat(argv[1], &info) == -1) {
        perror("stat");
        return 1;
    }

    printf("File: %s\n", argv[1]);
    print_file_type(info.st_mode);

    return 0;
}
```

---

### **2️⃣ Checking Permissions**

Each permission is represented by a specific bit in `st_mode`:

| Symbol | Meaning | Bitmask |
|:--:|:--|:--|
| `r` | Read | `S_IRUSR`, `S_IRGRP`, `S_IROTH` |
| `w` | Write | `S_IWUSR`, `S_IWGRP`, `S_IWOTH` |
| `x` | Execute | `S_IXUSR`, `S_IXGRP`, `S_IXOTH` |

#### **Example (Printing Permissions):**
```c
printf("Owner: %c%c%c\n",
       (info.st_mode & S_IRUSR) ? 'r' : '-',
       (info.st_mode & S_IWUSR) ? 'w' : '-',
       (info.st_mode & S_IXUSR) ? 'x' : '-');
```

---

### ✅ **Summary**
- `stat()` → info about the **target file**.  
- `lstat()` → info about the **symbolic link itself**.  
- `st_mode` → contains **file type + permission bits**.  
- Use macros like `S_ISDIR()`, `S_IRUSR` to check type and permissions.
