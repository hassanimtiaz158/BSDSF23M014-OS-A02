
#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>      // strdup, strlen
#include <sys/types.h>   // mode_t, uid_t, gid_t
#include <unistd.h>      // getopt, STDOUT_FILENO
#include <dirent.h>      // DIR, struct dirent
#include <sys/stat.h>    // stat, lstat
#include <pwd.h>         // getpwuid
#include <grp.h>         // getgrgid
#include <time.h>        // ctime, strftime
#include <sys/ioctl.h>   // ioctl, struct winsize
#include <errno.h>


#define MAX_FILES 1024

// Function to get terminal width
int get_terminal_width() {
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return w.ws_col;
    return 80; // Default fallback width
}

// Function to print file permissions
void print_permissions(mode_t mode) {
    char perms[11];
    perms[0] = S_ISDIR(mode) ? 'd' : '-';
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

// Long listing display (-l)
void display_long_listing(char *dirname, char **files, int count) {
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
        printf("%-8s ", getpwuid(st.st_uid)->pw_name);
        printf("%-8s ", getgrgid(st.st_gid)->gr_name);
        printf("%8ld ", st.st_size);

        char timebuf[64];
        strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));
        printf("%s ", timebuf);
        printf("%s\n", files[i]);
    }
}

// Default display (down then across)
void display_default(char **files, int count) {
    int maxlen = 0;
    for (int i = 0; i < count; i++)
        if ((int)strlen(files[i]) > maxlen)
            maxlen = strlen(files[i]);

    int term_width = get_terminal_width();
    int cols = term_width / (maxlen + 2);
    if (cols == 0) cols = 1;

    for (int i = 0; i < count; i++) {
        printf("%-*s", maxlen + 2, files[i]);
        if ((i + 1) % cols == 0)
            printf("\n");
    }
    printf("\n");
}

// New horizontal display (-x)
void display_horizontal(char **files, int count) {
    int maxlen = 0;
    for (int i = 0; i < count; i++)
        if ((int)strlen(files[i]) > maxlen)
            maxlen = strlen(files[i]);

    int term_width = get_terminal_width();
    int current_width = 0;

    for (int i = 0; i < count; i++) {
        int col_width = maxlen + 2;
        if (current_width + col_width > term_width) {
            printf("\n");
            current_width = 0;
        }
        printf("%-*s", col_width, files[i]);
        current_width += col_width;
    }
    printf("\n");
}

// Main listing function
void do_ls(char *dirname, int long_format, int horizontal) {
    DIR *dirp;
    struct dirent *entry;
    char *files[MAX_FILES];
    int count = 0;

    dirp = opendir(dirname);
    if (dirp == NULL) {
        perror("opendir");
        return;
    }

    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_name[0] != '.') { // skip hidden files
            files[count] = strdup(entry->d_name);
            count++;
        }
    }
    closedir(dirp);

    // Sort filenames alphabetically
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (strcmp(files[i], files[j]) > 0) {
                char *tmp = files[i];
                files[i] = files[j];
                files[j] = tmp;
            }
        }
    }

    // Display mode selection
    if (long_format)
        display_long_listing(dirname, files, count);
    else if (horizontal)
        display_horizontal(files, count);
    else
        display_default(files, count);

    for (int i = 0; i < count; i++)
        free(files[i]);
}

// Main function
int main(int argc, char *argv[]) {
    int long_format = 0;
    int horizontal = 0;
    char *dirname = ".";

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0)
            long_format = 1;
        else if (strcmp(argv[i], "-x") == 0)
            horizontal = 1;
        else
            dirname = argv[i];
    }

    do_ls(dirname, long_format, horizontal);
    return 0;
}
