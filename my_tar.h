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
struct tar_header {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char padding[355];
};
// Function Prototypes
void pad_block(int fd, int size);
int file_exists(const char *filename);
long my_strtol(const char *nptr, char **endptr, int base);
size_t my_strlen(const char *str);
void write_error(const char *msg);
int my_strcmp(const char *str1, const char *str2);
char *my_strncpy(char *dest, const char *src, size_t n);
void write_cannot_open_tarball_error(const char *filename);
void write_file_not_exist_error(const char *filename);
void *my_memset(void *ptr, int value, size_t n);
void my_itoa_octal_padded(char *buf, int value, int padding);
void fill_tar_header(struct tar_header *header, const char *filename, struct stat *st);
int add_file_to_tar(int tar_fd, const char *filename);
void list_tar_contents(int tar_fd);
int extract_files_from_tar(int tar_fd);
int append_file_to_tar(int tar_fd, const char *filename);
#endif // MY_TAR_H
