#ifndef MY_TAR_H
#define MY_TAR_H

#define _POSIX_C_SOURCE 200809L

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

#define BLOCK_SIZE 512 

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
void my_strncpy(char *dest, const char *src, int n);
int my_strcmp(const char *str1, const char *str2);
void my_memset(void *ptr, int value, size_t num);
void my_itoa_octal(int num, char *str, int size);
int my_strtol_octal(const char *str);
int create_tar_header(const char *file_name, tar_header *header);
int write_file_to_archive(int archive_fd, const char *file_name);
int list_archive_contents(int archive_fd);
int extract_archive(int archive_fd);
int main(int argc, char *argv[]);

#endif // MY_TAR_H