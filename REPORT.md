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







