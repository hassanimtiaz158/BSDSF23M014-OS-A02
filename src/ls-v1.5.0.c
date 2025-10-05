#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <sys/ioctl.h>

#define DEFAULT_TERM_WIDTH 80
#define SPACING 2

// -------------------
// ANSI color codes
// -------------------
#define COLOR_RESET      "\033[0m"
#define COLOR_RED        "\033[0;31m"
#define COLOR_GREEN      "\033[0;32m"
#define COLOR_BLUE       "\033[0;34m"
#define COLOR_MAGENTA    "\033[0;35m"
#define COLOR_CYAN       "\033[0;36m"
#define COLOR_WHITE      "\033[0;37m"
#define COLOR_PINK       "\033[1;35m"
#define COLOR_REVERSE    "\033[7m"
#define BOLD_BLUE        "\033[1;34m"
#define BOLD_GREEN       "\033[1;32m"

// -------------------
// Print file permissions like ls -l
// -------------------
void print_permissions(mode_t mode) {
    char perms[11];
    perms[0] = S_ISDIR(mode) ? 'd' :
               S_ISLNK(mode) ? 'l' :
               S_ISCHR(mode) ? 'c' :
               S_ISBLK(mode) ? 'b' :
               S_ISSOCK(mode) ? 's' :
               S_ISFIFO(mode) ? 'p' : '-';

    perms[1] = (mode & S_IRUSR) ? 'r' : '-';
    perms[2] = (mode & S_IWUSR) ? 'w' : '-';
    perms[3] = (mode & S_IXUSR) ? 'x' : '-';
    perms[4] = (mode & S_IRGRP) ? 'r' : '-';
    perms[5] = (mode & S_IWGRP) ? 'w' : '-';
    perms[6] = (mode & S_IXGRP) ? 'x' : '-';
    perms[7] = (mode & S_IROTH) ? 'r' : '-';
    perms[8] = (mode & S_IWOTH) ? 'w' : '-';
    perms[9] = (mode & S_IXOTH) ? 'x' : '-';
    perms[10] = '\0';

    printf("%s ", perms);
}

// -------------------
// Get terminal width
// -------------------
int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return w.ws_col;
    else
        return DEFAULT_TERM_WIDTH;
}

// -------------------
// Gather filenames and determine longest
// -------------------
char **gather_filenames(const char *path, int *count, size_t *longest) {
    DIR *d = opendir(path);
    if (!d) {
        perror("opendir");
        return NULL;
    }

    struct dirent *entry;
    char **filenames = NULL;
    *count = 0;
    *longest = 0;

    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue; // skip hidden files

        filenames = realloc(filenames, (*count + 1) * sizeof(char *));
        if (!filenames) {
            perror("realloc");
            closedir(d);
            return NULL;
        }

        filenames[*count] = strdup(entry->d_name);
        if (!filenames[*count]) {
            perror("strdup");
            closedir(d);
            return NULL;
        }

        size_t len = strlen(entry->d_name);
        if (len > *longest)
            *longest = len;

        (*count)++;
    }

    closedir(d);
    return filenames;
}

// -------------------
// Sorting comparison function
// -------------------
int cmpstr(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

// -------------------
// Print colored filename based on type and rules
// -------------------
void print_colored(const char *path, const char *name) {
    char fullpath[1024];
    snprintf(fullpath, sizeof(fullpath), "%s/%s", path, name);

    struct stat st;
    if (lstat(fullpath, &st) == -1) {
        perror("lstat");
        printf("%s", name);
        return;
    }

    // Symbolic links → Pink
    if (S_ISLNK(st.st_mode)) {
        printf(COLOR_PINK "%s" COLOR_RESET, name);
    }
    // Directories → Blue
    else if (S_ISDIR(st.st_mode)) {
        printf(BOLD_BLUE "%s" COLOR_RESET, name);
    }
    // Executables → Green
    else if (S_ISREG(st.st_mode) && (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))) {
        printf(BOLD_GREEN "%s" COLOR_RESET, name);
    }
    // Tarballs → Red
    else if (strstr(name, ".tar") || strstr(name, ".gz") || strstr(name, ".zip")) {
        printf(COLOR_RED "%s" COLOR_RESET, name);
    }
    // Special files → Reverse video
    else if (S_ISCHR(st.st_mode) || S_ISBLK(st.st_mode) || S_ISFIFO(st.st_mode) || S_ISSOCK(st.st_mode)) {
        printf(COLOR_REVERSE "%s" COLOR_RESET, name);
    }
    // Default
    else {
        printf("%s", name);
    }
}

// -------------------
// Display: Down-then-across (default)
// -------------------
void display_down_across(const char *path, char **filenames, int count, size_t longest) {
    int term_width = get_terminal_width();
    int col_width = longest + SPACING;
    int columns = term_width / col_width;
    if (columns < 1) columns = 1;

    int rows = (count + columns - 1) / columns;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            int index = c * rows + r;
            if (index < count) {
                print_colored(path, filenames[index]);
                printf("%-*s", (int)(col_width - strlen(filenames[index])), " ");
            }
        }
        printf("\n");
    }
}

// -------------------
// Display: Horizontal (-x)
// -------------------
void display_horizontal(const char *path, char **filenames, int count, size_t longest) {
    int term_width = get_terminal_width();
    int col_width = longest + SPACING;
    int current_width = 0;

    for (int i = 0; i < count; i++) {
        if (current_width + col_width > term_width) {
            printf("\n");
            current_width = 0;
        }
        print_colored(path, filenames[i]);
        printf("%-*s", (int)(col_width - strlen(filenames[i])), " ");
        current_width += col_width;
    }
    printf("\n");
}

// -------------------
// Display: Long listing (-l)
// -------------------
void display_long_listing(const char *path, char **filenames, int count) {
    for (int i = 0; i < count; i++) {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, filenames[i]);

        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            perror("lstat");
            continue;
        }

        print_permissions(st.st_mode);
        printf("%ld ", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf("%s %s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        printf("%5ld ", st.st_size);

        char *time_str = ctime(&st.st_mtime);
        time_str[strlen(time_str) - 1] = '\0';
        printf("%s ", time_str);

        print_colored(path, filenames[i]);
        printf("\n");
    }
}

// -------------------
// Main LS logic
// -------------------
int main(int argc, char *argv[]) {
    int opt;
    int long_format = 0;
    int horizontal = 0;

    while ((opt = getopt(argc, argv, "lx")) != -1) {
        switch (opt) {
            case 'l': long_format = 1; break;
            case 'x': horizontal = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-x] [dir]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";

    int count;
    size_t longest;
    char **filenames = gather_filenames(path, &count, &longest);
    if (!filenames || count == 0) return 0;

    // Sort alphabetically
    qsort(filenames, count, sizeof(char *), cmpstr);

    // Choose display mode
    if (long_format)
        display_long_listing(path, filenames, count);
    else if (horizontal)
        display_horizontal(path, filenames, count, longest);
    else
        display_down_across(path, filenames, count, longest);

    // Free memory
    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);

    return 0;
}

