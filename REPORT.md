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
