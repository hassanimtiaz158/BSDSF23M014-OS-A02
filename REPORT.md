# ANSI Escape Codes and Executable File Color Detection

## How ANSI Escape Codes Work

**ANSI escape codes** are special character sequences used to control formatting, color, and other output options in text terminals. They begin with the **escape character** (`\033` or `\x1B`) followed by `[` and a series of parameters ending with a letter that indicates the command.

For example, the general syntax for setting text color is:

```
\033[<style>;<text_color>;<background_color>m
```

Where:
- `<style>` sets attributes (e.g., `0` for reset, `1` for bold).
- `<text_color>` specifies the foreground color.
- `<background_color>` sets the background color (optional).

### Example: Printing Text in Green

To print text in **green**, you can use:

```c
printf("\033[32mThis text is green!\033[0m\n");
```

Explanation:
- `\033[32m` → Switches text color to green.
- `\033[0m` → Resets color and style back to default.

**Common color codes:**
| Color | Code |
|--------|------|
| Black | 30 |
| Red | 31 |
| Green | 32 |
| Yellow | 33 |
| Blue | 34 |
| Magenta | 35 |
| Cyan | 36 |
| White | 37 |

---

## Detecting Executable Files Using Permission Bits

In Linux, file permissions are stored in the `st_mode` field of the `struct stat` structure. You can check whether a file is **executable** using macros from `<sys/stat.h>`.

### Relevant Permission Bits:
- **Owner executable bit:** `S_IXUSR`
- **Group executable bit:** `S_IXGRP`
- **Others executable bit:** `S_IXOTH`

### Example Code:
```c
#include <sys/stat.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    struct stat st;
    if (stat(argv[1], &st) == -1) {
        perror("stat");
        return 1;
    }

    if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        printf("\033[32m%s\033[0m\n", argv[1]); // Green if executable
    } else {
        printf("%s\n", argv[1]); // Default color
    }

    return 0;
}
```

### Explanation:
- The program uses `stat()` to retrieve file metadata.
- It checks if any executable bit (`S_IXUSR`, `S_IXGRP`, or `S_IXOTH`) is set.
- If yes → prints the filename in **green**.
- Otherwise → prints it normally.

---

### ✅ **Summary**
- All directory entries must be read into memory to allow sorting algorithms like `qsort()` to access and rearrange them efficiently.
- This approach can be memory-heavy for very large directories.
- The `qsort()` function uses a comparison callback that accepts `const void *` arguments to support sorting of **any data type** safely and generically.


## **Feature:6**

# ANSI Escape Codes and Executable File Color Detection

## How ANSI Escape Codes Work

**ANSI escape codes** are special character sequences used to control formatting, color, and other output options in text terminals. They begin with the **escape character** (`\033` or `\x1B`) followed by `[` and a series of parameters ending with a letter that indicates the command.

For example, the general syntax for setting text color is:

```
\033[<style>;<text_color>;<background_color>m
```

Where:
- `<style>` sets attributes (e.g., `0` for reset, `1` for bold).
- `<text_color>` specifies the foreground color.
- `<background_color>` sets the background color (optional).

### Example: Printing Text in Green

To print text in **green**, you can use:

```c
printf("\033[32mThis text is green!\033[0m\n");
```

Explanation:
- `\033[32m` → Switches text color to green.
- `\033[0m` → Resets color and style back to default.

**Common color codes:**
| Color | Code |
|--------|------|
| Black | 30 |
| Red | 31 |
| Green | 32 |
| Yellow | 33 |
| Blue | 34 |
| Magenta | 35 |
| Cyan | 36 |
| White | 37 |

---

## Detecting Executable Files Using Permission Bits

In Linux, file permissions are stored in the `st_mode` field of the `struct stat` structure. You can check whether a file is **executable** using macros from `<sys/stat.h>`.

### Relevant Permission Bits:
- **Owner executable bit:** `S_IXUSR`
- **Group executable bit:** `S_IXGRP`
- **Others executable bit:** `S_IXOTH`

### Example Code:
```c
#include <sys/stat.h>
#include <stdio.h>

int main(int argc, char *argv[]) {
    struct stat st;
    if (stat(argv[1], &st) == -1) {
        perror("stat");
        return 1;
    }

    if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH)) {
        printf("\033[32m%s\033[0m\n", argv[1]); // Green if executable
    } else {
        printf("%s\n", argv[1]); // Default color
    }

    return 0;
}
```

### Explanation:
- The program uses `stat()` to retrieve file metadata.
- It checks if any executable bit (`S_IXUSR`, `S_IXGRP`, or `S_IXOTH`) is set.
- If yes → prints the filename in **green**.
- Otherwise → prints it normally.

---

### Summary
- **ANSI escape codes** modify terminal text color and style using sequences like `\033[32m`.
- **Executable detection** depends on checking the execute bits in `st_mode` using bitwise operations.
- Together, they enable color-coded listings similar to what the `ls` command does in Linux.


## **Feature:7**

# Report: Recursive `ls` Function Logic

## 1. What is a Base Case in Recursion?

A **base case** in a recursive function is the condition that **terminates the recursion**.  
It prevents the function from calling itself indefinitely, which would otherwise lead to an **infinite loop** and eventually a **stack overflow** error.

### Base Case in a Recursive `ls` Implementation
In a recursive version of the `ls` command, the function repeatedly lists directories and their subdirectories.  
The **base case** occurs when:
- The current directory contains **no subdirectories**, or  
- The entry being processed is either `"."` (current directory) or `".."` (parent directory).

These entries must be **skipped** to prevent infinite recursion into the same directories.

```c
if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
    continue; // Base case: stop recursion on these directories
```

This ensures that the recursion **stops naturally** when there are no further directories to process.

---

## 2. Why It’s Important to Construct a Full Path

When recursively calling the function to process subdirectories, it is essential to **build the complete path** — for example:

```
parent_dir/subdir
```

instead of just passing `"subdir"`.

### Reason:
Without the full path, the recursive function would look for `"subdir"` in the **current working directory**, not inside `"parent_dir"`.  
This would cause:
- **Incorrect traversal** (the function might open the wrong folder)
- **Errors** if `"subdir"` does not exist in the current directory
- **Incomplete or incorrect output**

### Correct Approach:
Construct the full path before making the recursive call:

```c
char path[1024];
snprintf(path, sizeof(path), "%s/%s", parent_dir, entry->d_name);
do_ls(path);
```

This ensures that:
- Each recursive call points to the **correct directory level**.
- The directory hierarchy is **preserved** and traversed properly.

---

## ✅ Summary

| Concept | Explanation |
|----------|--------------|
| **Base Case** | Stops recursion when no further subdirectories exist or when encountering `.` and `..`. Prevents infinite loops. |
| **Full Path Construction** | Ensures correct traversal by building `"parent_dir/subdir"` before recursive calls. Without it, the program may access wrong directories. |

By combining a proper **base case** and **full path construction**, the recursive `ls` implementation can safely and accurately traverse the entire directory tree.





