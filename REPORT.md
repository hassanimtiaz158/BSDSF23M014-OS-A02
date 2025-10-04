## Report Questions

### 1. Difference between `stat()` and `lstat()`

The crucial difference is how they handle **symbolic links**:

- **`stat(path, &buf)`**: Retrieves information about the file that `path` points to.  
  If `path` is a symbolic link, `stat()` follows the link and returns information about the target file.

- **`lstat(path, &buf)`**: Similar to `stat()`, but if `path` is a symbolic link, it retrieves information **about the link itself** (e.g., its size, permissions), not the target file.

**In the context of `ls`:**  
When implementing the `-l` long listing option, it is more appropriate to use **`lstat()`**. This allows `ls` to display details about the symbolic link itself, rather than only the file it points to. For example:

```bash
lrwxrwxrwx  1 user group    7 Oct  4 10:00 mylink -> target


# Report – Question 2

## Question
Explain how the `st_mode` field in the `stat` structure is used to determine the file type and permissions in a program. Provide code examples to demonstrate extracting file type and permissions.

---

## Understanding `st_mode`

The `st_mode` field in the `struct stat` returned by the `stat()` system call contains:

1. **File type information**  
2. **Permission bits**  

It is represented as an integer bitmask, and to decode it, we use **bitwise operators (`&`)** along with macros from `<sys/stat.h>`.

---

## File Type Extraction

The file type is determined using the `S_IFMT` mask. We compare it against constants:

- `S_IFREG` → Regular file  
- `S_IFDIR` → Directory  
- `S_IFLNK` → Symbolic link  
- `S_IFCHR` → Character device  
- `S_IFBLK` → Block device  
- `S_IFIFO` → FIFO/pipe  
- `S_IFSOCK` → Socket  

### Code Example (File Type):

```c
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>

void print_file_type(mode_t mode) {
    if ((mode & S_IFMT) == S_IFREG) printf("Regular file\n");
    else if ((mode & S_IFMT) == S_IFDIR) printf("Directory\n");
    else if ((mode & S_IFMT) == S_IFLNK) printf("Symbolic link\n");
    else if ((mode & S_IFMT) == S_IFCHR) printf("Character device\n");
    else if ((mode & S_IFMT) == S_IFBLK) printf("Block device\n");
    else if ((mode & S_IFMT) == S_IFIFO) printf("FIFO/pipe\n");
    else if ((mode & S_IFMT) == S_IFSOCK) printf("Socket\n");
    else printf("Unknown type\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    struct stat st;
    if (stat(argv[1], &st) == -1) {
        perror("stat");
        exit(1);
    }

    printf("File: %s\n", argv[1]);
    print_file_type(st.st_mode);

    return 0;
}


