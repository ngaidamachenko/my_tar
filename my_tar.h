#ifndef MY_TAR_H
#define MY_TAR_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <pwd.h>
#include <grp.h>
#include <utime.h>
#include <string.h>
#include <time.h>

// Complex and strict tar header that is close the original
typedef struct {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
} tar_header;


void write_error(const char *msg);
int my_strlen(const char *str);
void my_memset(void *ptr, int value, size_t num);
void my_itoa_octal(int value, char *str, size_t size);
int my_strcmp(const char *str1, const char *str2);
void create_archive(const char *archive_name, int argc, char *argv[]);
void append_to_archive(const char *archive_name, int argc, char *argv[]);
void list_archive_contents(const char *archive_name);
void extract_archive(int archive_fd);
int my_strtol_octal(const char *str, int size);
void add_file_to_archive(int archive_fd, const char *file_name);
int file_exists(const char *path);
void extract_file(int archive_fd, const tar_header *header);

#endif // MY_TAR_H
