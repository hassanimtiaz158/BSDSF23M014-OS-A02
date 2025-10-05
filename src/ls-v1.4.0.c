#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // strdup, strlen, strcmp
#include <sys/types.h>   // mode_t, uid_t, gid_t
#include <unistd.h>      // getopt, STDOUT_FILENO
#include <dirent.h>      // DIR, struct dirent
#include <sys/stat.h>    // stat, lstat
#include <pwd.h>         // getpwuid
#include <grp.h>         // getgrgid
#include <time.h>        // ctime, strftime
#include <sys/ioctl.h>   // ioctl, struct winsize
#include <errno.h>

#define SPACING 2
#define DEFAULT_TERM_WIDTH 80

// --- Function Prototypes ---
void print_permissions(mode_t mode);
int get_terminal_width();
void display_long_listing(const char *dirname, char **files, int count);
void display_default(char **files, int count);
void display_horizontal(char **files, int count);
void do_ls(const char *dirname, int long_format, int horizontal, int show_hidden);
int cmp_strings(const void *a, const void *b);

// --- Comparison function for qsort ---
int cmp_strings(const void *a, const void *b) {
    const char * const *sa = a;
    const char * const *sb = b;
    return strcmp(*sa, *sb);
}

// --- Print file permissions like ls -l ---
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

// --- Get terminal width ---
int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return w.ws_col;
    return DEFAULT_TERM_WIDTH;
}

// --- Long listing (-l) ---
void display_long_listing(const char *dirname, char **files, int count) {
    for (int i = 0; i < count; i++) {
        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", dirname, files[i]);
        struct stat st;
        if (stat(path, &st) == -1) {
            perror("stat");
            continue;
        }

        print_permissions(st.st_mode);
        printf("%3ld ", st.st_nlink);

        struct passwd *pw = getpwuid(st.st_uid);
        struct group *gr = getgrgid(st.st_gid);
        printf("%-8s %-8s ", pw ? pw->pw_name : "?", gr ? gr->gr_name : "?");

        printf("%8ld ", st.st_size);

        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
        printf("%s ", timebuf);
        printf("%s\n", files[i]);
    }
}

// --- Default display (down then across) ---
void display_default(char **files, int count) {
    int maxlen = 0;
    for (int i = 0; i < count; i++)
        if ((int)strlen(files[i]) > maxlen)
            maxlen = strlen(files[i]);

    int term_width = get_terminal_width();
    int col_width = maxlen + SPACING;
    int columns = term_width / col_width;
    if (columns < 1) columns = 1;

    int rows = (count + columns - 1) / columns;

    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            int idx = c * rows + r;
            if (idx < count)
                printf("%-*s", col_width, files[idx]);
        }
        printf("\n");
    }
}

// --- Horizontal display (-x) ---
void display_horizontal(char **files, int count) {
    int maxlen = 0;
    for (int i = 0; i < count; i++)
        if ((int)strlen(files[i]) > maxlen)
            maxlen = strlen(files[i]);

    int term_width = get_terminal_width();
    int current_width = 0;
    int col_width = maxlen + SPACING;

    for (int i = 0; i < count; i++) {
        if (current_width + col_width > term_width) {
            printf("\n");
            current_width = 0;
        }
        printf("%-*s", col_width, files[i]);
        current_width += col_width;
    }
    printf("\n");
}

// --- Main listing function ---
void do_ls(const char *dirname, int long_format, int horizontal, int show_hidden) {
    DIR *dirp = opendir(dirname);
    if (!dirp) {
        perror("opendir");
        return;
    }

    struct dirent *entry;
    char **files = NULL;
    int count = 0;

    while ((entry = readdir(dirp)) != NULL) {
        // Skip hidden files unless show_hidden is true
        if (!show_hidden && entry->d_name[0] == '.')
            continue;

        char *name = strdup(entry->d_name);
        if (!name) {
            perror("strdup");
            closedir(dirp);
            for (int i = 0; i < count; i++)
                free(files[i]);
            free(files);
            return;
        }

        char **tmp = realloc(files, (count + 1) * sizeof(char *));
        if (!tmp) {
            perror("realloc");
            free(name);
            closedir(dirp);
            for (int i = 0; i < count; i++)
                free(files[i]);
            free(files);
            return;
        }
        files = tmp;
        files[count++] = name;
    }
    closedir(dirp);

    // --- Sort filenames alphabetically ---
    qsort(files, count, sizeof(char *), cmp_strings);

    // --- Display mode selection ---
    if (long_format)
        display_long_listing(dirname, files, count);
    else if (horizontal)
        display_horizontal(files, count);
    else
        display_default(files, count);

    // --- Free memory ---
    for (int i = 0; i < count; i++)
        free(files[i]);
    free(files);
}

// --- Main function ---
int main(int argc, char *argv[]) {
    int long_format = 0;
    int horizontal = 0;
    int show_hidden = 0;
    const char *dirname = ".";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            long_format = 1;
        else if (strcmp(argv[i], "-x") == 0)
            horizontal = 1;
        else if (strcmp(argv[i], "-a") == 0)
            show_hidden = 1; // prepared for future -a flag
        else
            dirname = argv[i];
    }

    do_ls(dirname, long_format, horizontal, show_hidden);
    return 0;
}
