
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
