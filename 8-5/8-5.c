#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

#define MAX_PATH_LEN 1024

void fsize(char *name);
void dir_walk(char *dir_name, void (*func)(char *));

void print_file_flags(mode_t st_mode);
void print_file_user(uid_t st_uid);
void print_file_group(gid_t st_gid);
void print_file_size(size_t size);
void print_file_time(time_t time);

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        fsize(".");
    }
    else
    {
        while (--argc > 0)
        {
            fsize(*++argv);
        }
    }

    return 0;
}

void fsize(char *name)
{
    struct stat buffer;

    if (stat(name, &buffer) == -1)
    {
        fprintf(stderr, "fsize: cannot access %s\n", name);
        return;
    }

    if ((buffer.st_mode & S_IFMT) == S_IFDIR)
    {
        dir_walk(name, fsize);
    }

    print_file_flags(buffer.st_mode);
    printf("%lu ", buffer.st_nlink);
    print_file_user(buffer.st_uid);
    print_file_group(buffer.st_gid);
    print_file_size(buffer.st_size);
    print_file_time(buffer.st_atime);
    printf("%s\n", name);
}

void dir_walk(char *dir_name, void (*func)(char *))
{
    char name[MAX_PATH_LEN];
    struct dirent *dir_entry;
    DIR *dir;

    if ((dir = opendir(dir_name)) == NULL)
    {
        fprintf(stderr, "dir_walk: cannot open %s\n", dir_name);
        return;
    }

    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (strcmp(dir_entry->d_name, ".") == 0 || strcmp(dir_entry->d_name, "..") == 0)
        {
            continue;
        }

        if (strlen(dir_name) + strlen(dir_entry->d_name) + 2 > MAX_PATH_LEN)
        {
            fprintf(stderr, "dir_walk: path too long\n");
        }
        else
        {
            sprintf(name, "%s/%s", dir_name, dir_entry->d_name);
            (*func)(name);
        }
    }

    closedir(dir);
}

void print_file_flags(mode_t st_mode)
{
    printf("%c", ((st_mode & S_IFMT) == S_IFDIR) ? 'd' : '-');
    printf("%c", (st_mode & S_IRUSR) ? 'r' : '-');
    printf("%c", (st_mode & S_IWUSR) ? 'w' : '-');
    printf("%c", (st_mode & S_IXUSR) ? 'x' : '-');
    printf("%c", (st_mode & S_IRGRP) ? 'r' : '-');
    printf("%c", (st_mode & S_IWGRP) ? 'w' : '-');
    printf("%c", (st_mode & S_IXGRP) ? 'x' : '-');
    printf("%c", (st_mode & S_IROTH) ? 'r' : '-');
    printf("%c", (st_mode & S_IWOTH) ? 'w' : '-');
    printf("%c ", (st_mode & S_IXOTH) ? 'x' : '-');
}

void print_file_user(uid_t st_uid)
{
    struct passwd *password;
    password = getpwuid(st_uid);

    if (password == NULL)
    {
        fprintf(stderr, "Error: cannot find user\n");
        return;
    }

    printf("%s ", password->pw_name);
}

void print_file_group(gid_t st_gid)
{
    struct group *group;
    group = getgrgid(st_gid);

    if (group == NULL)
    {
        fprintf(stderr, "Error: cannot find group\n");
        return;
    }

    printf("%s ", group->gr_name);
}

void print_file_size(size_t size)
{
    static const char *SIZES[] = {"B", "K", "M", "G"};
    size_t div = 0;
    size_t rem = 0;

    while (size >= 1024 && div < (sizeof SIZES / sizeof *SIZES))
    {
        rem = (size % 1024);
        div++;
        size /= 1024;
    }

    printf("%6.1f%s ", (float)size + (float)rem / 1024.0, SIZES[div]);
}

void print_file_time(time_t time)
{
    char time_str[32];
    strftime(time_str, sizeof(time_str), "%d %b %H:%M", localtime(&time));
    printf("%s ", time_str);
}