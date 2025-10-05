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

