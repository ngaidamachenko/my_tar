#include "my_tar.h"
// Zero-padded block
void pad_block(int fd, int size) {
    char zero[BLOCK_SIZE] = {0};
    int pad_size = BLOCK_SIZE - (size % BLOCK_SIZE);
    if (pad_size < BLOCK_SIZE) {
        write(fd, zero, pad_size);
    }
} 
int file_exists(const char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}
long my_strtol(const char *nptr, char **endptr, int base) {
    const char *ptr = nptr;
    long result = 0;
    int sign = 1;
    // Skip leading whitespace
    while (*ptr == ' ' || (*ptr >= 9 && *ptr <= 13)) {
        ptr++;
    }
    // Handle optional sign
    if (*ptr == '-') {
        sign = -1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }
    // Handle base prefixes
    if (base == 0) {
        if (*ptr == '0') {
            if (*(ptr + 1) == 'x' || *(ptr + 1) == 'X') {
                base = 16; // Hexadecimal
                ptr += 2;
            } else {
                base = 8; // Octal
                ptr++;
            }
        } else {
            base = 10; // Decimal
        }
    } else if (base < 2 || base > 36) {
        // Invalid base
        if (endptr) *endptr = (char *)nptr;
        return 0; // Return 0 for invalid base
    }
    // Convert to integer
    while ((*ptr >= '0' && *ptr <= '9' && (*ptr - '0') < base) ||
           (*ptr >= 'A' && *ptr <= 'Z' && (*ptr - 'A' + 10) < base) ||
           (*ptr >= 'a' && *ptr <= 'z' && (*ptr - 'a' + 10) < base)) {
        if (*ptr >= '0' && *ptr <= '9') {
            result = result * base + (*ptr - '0');
        } else if (*ptr >= 'A' && *ptr <= 'Z') {
            result = result * base + (*ptr - 'A' + 10);
        } else {
            result = result * base + (*ptr - 'a' + 10);
        }
        ptr++;
    }
    // Apply the sign
    result *= sign;
    // Set the endptr if it's not NULL
    if (endptr) {
        *endptr = (char *)ptr; // Pointer to the character that stopped the conversion
    }
    return result;
}
size_t my_strlen(const char *str) {
    size_t length = 0;
    while (str[length] != '\0') {
        length++;
    }
    return length;
}
void write_error(const char *msg) {
    write(2, msg, my_strlen(msg));
}
int my_strcmp(const char *str1, const char *str2) {
    size_t i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return (unsigned char)str1[i] - (unsigned char)str2[i];
        }
        i++;
    }
    return (unsigned char)str1[i] - (unsigned char)str2[i];
}
char *my_strncpy(char *dest, const char *src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}
void write_cannot_open_tarball_error(const char *filename) {
    const char *prefix = "my_tar: ";
    const char *prefix1 = ": Cannot open";
    const char *suffix = "\n";
    write(2, prefix, my_strlen(prefix));
    write(2, prefix1, my_strlen(prefix1));
    write(2, filename, my_strlen(filename));
    write(2, suffix, my_strlen(suffix));
}
void write_file_not_exist_error(const char *filename) {
    const char *prefix = "my_tar: ";
    const char *suffix = ": Cannot stat: No such file or directory\n";
    write(2, prefix, my_strlen(prefix));
    write(2, filename, my_strlen(filename));
    write(2, suffix, my_strlen(suffix));
}
void *my_memset(void *ptr, int value, size_t n) {
    unsigned char *p = ptr;
    for (size_t i = 0; i < n; i++) {
        p[i] = (unsigned char)value;
    }
    return ptr;
}
void my_itoa_octal_padded(char *buf, int value, int padding) {
    int i = padding - 1;
    buf[i--] = '\0';
    do {
        buf[i--] = '0' + (value & 7);
        value >>= 3;
    } while (value > 0 && i >= 0);
    while (i >= 0) {
        buf[i--] = '0';
    }
}
void fill_tar_header(struct tar_header *header, const char *filename, struct stat *st) {
    my_memset(header, 0, sizeof(struct tar_header));
    my_strncpy(header->name, filename, 100);
    my_itoa_octal_padded(header->mode, st->st_mode & 0777, 7);
    my_itoa_octal_padded(header->uid, st->st_uid, 7);
    my_itoa_octal_padded(header->gid, st->st_gid, 7);
    my_itoa_octal_padded(header->size, (unsigned int)st->st_size, 11);
    my_itoa_octal_padded(header->mtime, (unsigned int)st->st_mtime, 11);
    header->typeflag = '0';
    my_memset(header->chksum, ' ', sizeof(header->chksum));
    unsigned int chksum = 0;
    for (size_t i = 0; i < sizeof(struct tar_header); i++) {
        chksum += ((unsigned char*)header)[i];
    }
    my_itoa_octal_padded(header->chksum, chksum, 6);
}
int add_file_to_tar(int tar_fd, const char *filename) {
    int file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        if (!file_exists(filename)) {
            write_file_not_exist_error(filename);
        } else {
            write_error("open");
        }
        return -1;
    }
    struct stat st;
    if (fstat(file_fd, &st) < 0) {
        write_error("fstat");
        close(file_fd);
        return -1;
    }
    struct tar_header header;
    fill_tar_header(&header, filename, &st);
    if (write(tar_fd, &header, sizeof(header)) != sizeof(header)) {
        write_error("write");
        close(file_fd);
        return -1;
    }
    char buffer[BLOCK_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, sizeof(buffer))) > 0) {
        if (write(tar_fd, buffer, bytes_read) != bytes_read) {
            write_error("write");
            close(file_fd);
            return -1;
        }
    }
    pad_block(tar_fd, st.st_size);
    close(file_fd);
    return 0;
}
void list_tar_contents(int tar_fd) {
    struct tar_header header;
    while (read(tar_fd, &header, sizeof(header)) > 0) {
        if (header.name[0] == '\0') {
            break;
        }
        //printf("%s\n", header.name);
         write(STDOUT_FILENO, header.name, my_strlen(header.name));
         write(STDOUT_FILENO, "\n", 1);
        int file_size = my_strtol(header.size, NULL, 8);
        lseek(tar_fd, ((file_size + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE, SEEK_CUR);
    }
}
int extract_files_from_tar(int tar_fd) {
    struct tar_header header;
    char buffer[BLOCK_SIZE];
    while (read(tar_fd, &header, sizeof(header)) > 0) {
        if (header.name[0] == '\0') {
            break;
        }
        int file_fd = open(header.name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (file_fd < 0) {
            write_error("open");
            return 1;
        }
        int file_size = my_strtol(header.size, NULL, 8);
        for (int remaining = file_size; remaining > 0; ) {
            int bytes_to_read = (remaining > BLOCK_SIZE) ? BLOCK_SIZE : remaining;
            read(tar_fd, buffer, bytes_to_read);
            write(file_fd, buffer, bytes_to_read);
            remaining -= bytes_to_read;
        }
        close(file_fd);
        int padding = (BLOCK_SIZE - (file_size % BLOCK_SIZE)) % BLOCK_SIZE;
        lseek(tar_fd, padding, SEEK_CUR);
    }
    return 0;
}
int append_file_to_tar(int tar_fd, const char *filename) {
    struct tar_header header;
    char buffer[BLOCK_SIZE];
    while (read(tar_fd, &header, sizeof(header)) > 0) {
        if (header.name[0] == '\0') {
            if (read(tar_fd, buffer, BLOCK_SIZE) == BLOCK_SIZE && buffer[0] == '\0') {
                lseek(tar_fd, -2 * BLOCK_SIZE, SEEK_CUR);
                break;
            }
        } else {
            int file_size = my_strtol(header.size, NULL, 8);
            lseek(tar_fd, ((file_size + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE, SEEK_CUR);
        }
    }
    return add_file_to_tar(tar_fd, filename);
}
int main(int argc, char *argv[]) {
    if (argc < 3) {
        write_error("Usage: my_tar -c|-r|-t|-u|-x -f archive_name [file...]\n");
        return 1;
    }
    char *mode = argv[1];
    int c = 0;
    if ((my_strcmp(mode, "-c") == 0) || (my_strcmp(mode, "-x") == 0) || (my_strcmp(mode, "-t") == 0) || (my_strcmp(mode, "-r") == 0)) {
        c = 3;
    } else {
        c = 2;
    }
    char *tar_filename = argv[c];
    int tar_fd;
    c = c + 1;
    if ((my_strcmp(mode, "-cf") == 0) || (my_strcmp(mode, "-c") == 0)) {
        tar_fd = open(tar_filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (tar_fd < 0) {
            write_error("open");
            write_cannot_open_tarball_error(tar_filename);
            return 1;
        }
        for (int i = c; i < argc; i++) {
            if (add_file_to_tar(tar_fd, argv[i]) < 0) {
                close(tar_fd);
                return 1;
            }
        }
        close(tar_fd);
// In the main function for the -r option
} else if ((my_strcmp(mode, "-rf") == 0) || (my_strcmp(mode, "-r") == 0)) {
    tar_fd = open(tar_filename, O_RDWR);
    if (tar_fd < 0) {
        write_cannot_open_tarball_error(tar_filename);
        return 1;
    }
    for (int i = c; i < argc; i++) {
        if (append_file_to_tar(tar_fd, argv[i]) < 0) {
            close(tar_fd);
            return 1;
        }
    }
    close(tar_fd);
}
else if ((my_strcmp(mode, "-tf") == 0) || (my_strcmp(mode, "-t") == 0)) {
        tar_fd = open(tar_filename, O_RDONLY);
        if (tar_fd < 0) {
            write_cannot_open_tarball_error(tar_filename);
            return 1;
        }
        list_tar_contents(tar_fd);
        close(tar_fd);
    } else if ((my_strcmp(mode, "-xf") == 0) || (my_strcmp(mode, "-x") == 0)) {
        tar_fd = open(tar_filename, O_RDONLY);
        if (tar_fd < 0) {
            write_cannot_open_tarball_error(tar_filename);
            return 1;
        }
        if (extract_files_from_tar(tar_fd) < 0) {
            close(tar_fd);
            return 1;
        }
        close(tar_fd);
    } else {
        return 1;
    }
    return 0;
}
