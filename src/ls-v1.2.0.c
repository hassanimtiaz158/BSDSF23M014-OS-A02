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
#include <sys/ioctl.h>   // <-- ADD THIS LINE

#define DEFAULT_TERM_WIDTH 80
#define SPACING 2

// Print file permissions like ls -l
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

// Get terminal width using ioctl (fallback to default)
int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return w.ws_col;
    else
        return DEFAULT_TERM_WIDTH;
}

// Gather all filenames in directory and find longest filename
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

// Simple listing with "Down Then Across" logic
void list_simple(const char *path) {
    int count;
    size_t longest;
    char **filenames = gather_filenames(path, &count, &longest);
    if (!filenames || count == 0) return;

    int term_width = get_terminal_width();
    int col_width = longest + SPACING;
    int columns = term_width / col_width;
    if (columns < 1) columns = 1;

    int rows = (count + columns - 1) / columns; // ceiling division

    // ---- DOWN THEN ACROSS printing ----
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            int index = c * rows + r; // go down first, then across
            if (index < count)
                printf("%-*s", (int)col_width, filenames[index]);
        }
        printf("\n");
    }

    for (int i = 0; i < count; i++)
        free(filenames[i]);
    free(filenames);
}

// Long listing (like ls -l)
void list_long(const char *path) {
    int count;
    size_t longest;
    char **filenames = gather_filenames(path, &count, &longest);
    if (!filenames) return;

    for (int i = 0; i < count; i++) {
        char fullpath[1024];
        snprintf(fullpath, sizeof(fullpath), "%s/%s", path, filenames[i]);

        struct stat st;
        if (lstat(fullpath, &st) == -1) {
            perror("lstat");
            free(filenames[i]);
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

        printf("%s\n", filenames[i]);
        free(filenames[i]);
    }

    free(filenames);
}

int main(int argc, char *argv[]) {
    int opt;
    int long_format = 0;

    while ((opt = getopt(argc, argv, "l")) != -1) {
        switch (opt) {
            case 'l':
                long_format = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-l] [dir]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    const char *path = (optind < argc) ? argv[optind] : ".";

    if (long_format)
        list_long(path);
    else
        list_simple(path);

    return 0;
}
