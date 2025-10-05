## **Report Questions**

---
## **Feature:2**

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


## **Feature:3**

## **Report Questions: Columnar Output and ioctl System Call**

---

### **1. Logic for Printing Items in a 'Down Then Across' Columnar Format**

When printing items (like filenames) in a columnar format similar to the Unix `ls` command, the goal is to display the list efficiently across multiple columns based on the terminal width.

#### **General Logic:**
1. **Determine the number of rows and columns:**
   - Find the **longest filename length**.
   - Use the **terminal width** to calculate how many columns can fit.
   - Compute the number of rows needed to display all items.

2. **Print items down each column:**
   - Unlike the usual row-major order (across first), we print **down each column** before moving to the next.
   - For example, if we have 10 filenames and 3 columns, we fill them as:
     ```
     file1   file4   file7
     file2   file5   file8
     file3   file6   file9
     ```

3. **Iterate carefully:**
   - A single loop (e.g., `for i in range(n)`) won’t work because it only processes items linearly, across one dimension.
   - You need **nested loops**: one for rows and one for columns, calculating the correct index (`index = row + column * rows`).

#### **Why a Single Loop is Insufficient:**
A simple loop prints filenames one after another (across a single line or column). It cannot manage:
- The layout structure (rows × columns)
- Proper alignment of filenames
- Wrapping based on terminal width

Therefore, **two-dimensional indexing** is required to print “down then across.”

---

### **2. Purpose of the `ioctl` System Call in This Context**

The `ioctl` system call is used to **query or control device parameters**. In this context (like in the `ls` implementation), it helps detect the **current terminal size** — specifically, the **number of columns (width)**.

#### **How It’s Used:**
```c
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

struct winsize w;
ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
printf("Terminal width: %d columns\n", w.ws_col);
```
- `STDOUT_FILENO`: the file descriptor for the terminal output
- `TIOCGWINSZ`: request code to get the window size
- `w.ws_col`: gives the number of columns in the terminal

This allows the program to automatically adjust column layout based on the current terminal size.

---

### **3. Limitations of Using a Fixed-Width Fallback (e.g., 80 Columns)**
If the program doesn’t use `ioctl` and instead assumes a **fixed width**, such as 80 columns:

- On **wider terminals**, output will waste space (few columns used).
- On **narrow terminals**, output may wrap or look misaligned.
- It won’t adapt if the user resizes the terminal window.

#### **Summary of Limitations:**
| Problem | Description |
|----------|--------------|
| Lack of adaptability | Layout doesn’t adjust to current screen size |
| Poor readability | Lines may wrap or appear uneven |
| Inefficient use of space | Wastes space on wide terminals |

Using `ioctl` provides a **dynamic and user-friendly layout** that matches the actual terminal dimensions, making output cleaner and more responsive.


## **Feature:4**

## **Report Questions: Display Mode Logic and Implementation Complexity**

---

### **1. Comparison: 'Down Then Across' (Vertical) vs 'Across' (Horizontal) Printing Logic**

When implementing a file listing program (like `ls`), the output can be formatted either **across** (row-wise) or **down then across** (column-wise). These two approaches differ significantly in implementation complexity.

#### **Horizontal ('Across') Printing Logic:**
- This is the **simpler** approach.
- The program prints filenames **sequentially** across the screen until it reaches the end of a row (based on terminal width), then moves to the next line.
- Only basic spacing and wrapping calculations are needed.

##### **Example (across):**
```
file1  file2  file3  file4
file5  file6  file7  file8
```

**Complexity:** Minimal. A single loop over filenames is enough.

---

#### **Vertical ('Down Then Across') Printing Logic:**
- This method prints items **down each column first**, then moves across to the next column.
- To do this, the program must **precompute**:
  - The **number of columns** that fit in the terminal.
  - The **number of rows** needed to display all items.
  - The **exact index** of each item in the list (`index = row + column * rows`).
- The logic requires **nested loops** and careful indexing to avoid missing or repeating entries.

##### **Example (down then across):**
```
file1  file4  file7
file2  file5  file8
file3  file6  file9
```

**Complexity:** Higher — requires more pre-calculation of column count, row count, and precise index mapping.

---

#### **Summary of Complexity Comparison:**
| Format | Implementation Complexity | Reason |
|---------|-----------------------------|---------|
| Across (Horizontal) | Low | Sequential printing using a single loop |
| Down then Across (Vertical) | High | Requires pre-calculation of rows, columns, and index mapping based on terminal width |

In short, **vertical printing** is more complex because it must organize filenames in a 2D grid before printing, while **horizontal printing** only needs linear iteration.

---

### **2. Strategy for Managing Display Modes (-l, -x, and Default)**

In a typical `ls`-like implementation, different display modes change how files are shown:

| Mode | Description |
|------|--------------|
| `-l` | Long listing (detailed file info like permissions, owner, size, date) |
| `-x` | Across (horizontal) column layout |
| Default | Down-then-across column layout |

#### **Strategy Used:**
1. **Parse command-line options** using `getopt()` or manual string checks.
2. **Store mode flags** (e.g., `flag_l`, `flag_x`, `flag_default`).
3. **Decide which print function to call** based on the active flag.

##### **Example Pseudocode:**
```c
if (flag_l)
    print_long_format(file_list);
else if (flag_x)
    print_across(file_list);
else
    print_down_across(file_list);
```

Each printing function handles formatting independently:
- `print_long_format()` → calls `lstat()` and displays permissions, size, owner, etc.
- `print_across()` → prints filenames row-wise (simple spacing logic).
- `print_down_across()` → prints filenames in vertical columns using precomputed layout.

This modular approach keeps each display mode **separate and maintainable**, while the main function only decides **which mode to trigger**.

---

### ✅ **Summary**
- The **'down then across'** logic is more complex because it needs **pre-calculation** of grid structure (rows, columns, and indices).
- The **'across'** logic is simpler — just a single loop with spacing.
- The program determines display mode (`-l`, `-x`, or default) through **flags**, and calls the corresponding **print function** to handle that format independently.






