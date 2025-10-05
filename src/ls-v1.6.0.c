#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <errno.h>
#include <sys/ioctl.h>

// ---------------- CONFIG -----------------
#define DEFAULT_TERM_WIDTH 80
#define SPACING 2

// ANSI colors
#define COLOR_BLUE     "\033[0;34m"
#define COLOR_GREEN    "\033[0;32m"
#define COLOR_RED      "\033[0;31m"
#define COLOR_MAGENTA  "\033[0;35m"
#define COLOR_RESET    "\033[0m"
#define COLOR_REVERSE  "\033[7m"

// ---------------- HELPERS -----------------
int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return w.ws_col;
    return DEFAULT_TERM_WIDTH;
}

int compare_strings(const void *a, const void *b) {
    char *sa = *(char **)a;
    char *sb = *(char **)b;
    return strcmp(sa, sb);
}

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

void print_colored(const char *name, mode_t mode) {
    if (S_ISDIR(mode)) printf(COLOR_BLUE "%s" COLOR_RESET, name);
    else if (S_ISLNK(mode)) printf(COLOR_MAGENTA "%s" COLOR_RESET, name);
    else if (mode & S_IXUSR) printf(COLOR_GREEN "%s" COLOR_RESET, name);
    else if (strstr(name, ".tar") || strstr(name, ".gz") || strstr(name, ".zip"))
        printf(COLOR_RED "%s" COLOR_RESET, name);
    else if (S_ISCHR(mode) || S_ISBLK(mode) || S_ISSOCK(mode) || S_ISFIFO(mode))
        printf(COLOR_REVERSE "%s" COLOR_RESET, name);
    else
        printf("%s", name);
}

// ---------------- GATHER FILES -----------------
char **gather_filenames(const char *path, int *count, size_t *longest) {
    DIR *d = opendir(path);
    if (!d) { perror("opendir"); return NULL; }

    struct dirent *entry;
    char **files = NULL;
    *count = 0;
    *longest = 0;

    while ((entry = readdir(d)) != NULL) {
        if (entry->d_name[0] == '.') continue; // skip hidden
        files = realloc(files, (*count + 1) * sizeof(char *));
        if (!files) { perror("realloc"); closedir(d); return NULL; }
        files[*count] = strdup(entry->d_name);
        if (!files[*count]) { perror("strdup"); closedir(d); return NULL; }

        size_t len = strlen(entry->d_name);
        if (len > *longest) *longest = len;
        (*count)++;
    }

    closedir(d);
    qsort(files, *count, sizeof(char *), compare_strings);
    return files;
}

// ---------------- DISPLAY -----------------
void display_long_listing(const char *path, char **files, int count) {
    for (int i = 0; i < count; i++) {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, files[i]);
        struct stat st;
        if (lstat(fullpath, &st) == -1) continue;

        print_permissions(st.st_mode);
        printf("%ld ", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf("%s %s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        printf("%5ld ", st.st_size);

        char *time_str = ctime(&st.st_mtime);
        time_str[strlen(time_str)-1] = '\0';
        printf("%s ", time_str);

        print_colored(files[i], st.st_mode);
        printf("\n");
    }
}

void display_vertical(const char *path, char **files, int count, size_t longest) {
    int term_width = get_terminal_width();
    int col_width = longest + SPACING;
    int columns = term_width / col_width;
    if (columns < 1) columns = 1;
    int rows = (count + columns - 1) / columns;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            int idx = c * rows + r;
            if (idx < count) {
                char fullpath[1024];
                snprintf(fullpath, sizeof(fullpath), "%s/%s", path, files[idx]);
                struct stat st;
                if (lstat(fullpath, &st) == -1) continue;

                print_colored(files[idx], st.st_mode);
                printf("%-*s", (int)(col_width + strlen(files[idx]) - strlen(files[idx])), "");
            }
        }
        printf("\n");
    }
}

void display_horizontal(const char *path, char **files, int count, size_t longest) {
    int term_width = get_terminal_width();
    int col_width = longest + SPACING;
    int current_width = 0;

    for (int i = 0; i < count; i++) {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, files[i]);
        struct stat st;
        if (lstat(fullpath, &st) == -1) continue;

        print_colored(files[i], st.st_mode);
        int len = strlen(files[i]) + SPACING;
        current_width += len;
        if (current_width >= term_width) {
            printf("\n");
            current_width = len;
        } else {
            printf("%*s", SPACING, "");
        }
    }
    printf("\n");
}

// ----------------- RECURSIVE LS -----------------
void do_ls(const char *path, int long_format, int horizontal_flag, int recursive_flag) {
    int count;
    size_t longest;
    char **files = gather_filenames(path, &count, &longest);
    if (!files || count == 0) return;

    printf("%s:\n", path);

    if (long_format)
        display_long_listing(path, files, count);
    else if (horizontal_flag)
        display_horizontal(path, files, count, longest);
    else
        display_vertical(path, files, count, longest);

    if (recursive_flag) {
        for (int i = 0; i < count; i++) {
            char fullpath[1024];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, files[i]);

            struct stat st;
            if (lstat(fullpath, &st) == -1) continue;

            if (S_ISDIR(st.st_mode) && strcmp(files[i], ".") != 0 && strcmp(files[i], "..") != 0) {
                printf("\n");
                do_ls(fullpath, long_format, horizontal_flag, recursive_flag);
            }
        }
    }

    for (int i = 0; i < count; i++) free(files[i]);
    free(files);
}

// ----------------- MAIN -----------------
int main(int argc, char *argv[]) {
    int opt;
    int long_format = 0;
    int horizontal_flag = 0;
    int recursive_flag = 0;

    while ((opt = getopt(argc, argv, "lRx")) != -1) {
        switch(opt) {
            case 'l': long_format = 1; break;
            case 'x': horizontal_flag = 1; break;
            case 'R': recursive_flag = 1; break;
            default:
                fprintf(stderr, "Usage: %s [-l] [-x] [-R] [dir]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";

    do_ls(path, long_format, horizontal_flag, recursive_flag);

    return 0;
}
