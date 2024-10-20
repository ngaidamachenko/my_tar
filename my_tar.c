#include "my_tar.h"

// Write error messages using write to stderr
void write_error(const char *msg) {
    write(2, msg, my_strlen(msg));
}

// Custom strlen function
int my_strlen(const char *str) {
    int len = 0;
    while (str[len] != '\0') {
        len++;
    }
    return len;
}

// Custom function to copy strings safely
void my_strncpy(char *dest, const char *src, int n) {
    for (int i = 0; i < n; i++) {
        if (src[i] != '\0') {
            dest[i] = src[i];
        } else {
            dest[i] = '\0';
        }
    }
}

// Custom function to compare strings like strcmp
int my_strcmp(const char *str1, const char *str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return str1[i] - str2[i];
        }
        i++;
    }
    return str1[i] - str2[i];  // Compare null terminator
}

// Custom function to set memory to a value similar to memset
void my_memset(void *ptr, int value, size_t num) {
    unsigned char *p = ptr;
    for (size_t i = 0; i < num; i++) {
        p[i] = (unsigned char)value;
    }
}

// Custom function to convert integer to octal string
void my_itoa_octal(int num, char *str, int size) {
    for (int i = size - 1; i >= 0; i--) {
        str[i] = '0' + (num & 7);  // Get the last 3 bits (octal representation)
        num >>= 3;
    }
}

// Custom function to convert octal string to integer similar to sscanf for octal
int my_strtol_octal(const char *str) {
    int result = 0;
    while (*str >= '0' && *str <= '7') {
        result = (result << 3) + (*str - '0');  // Shift left by 3 (octal base)
        str++;
    }
    return result;
}

// Create tar header for a file
int create_tar_header(const char *file_name, tar_header *header) {
    struct stat st;
    if (lstat(file_name, &st) < 0) {
        write_error("my_tar: Cannot stat file\n");
        return -1;
    }

    my_memset(header, 0, sizeof(tar_header));  // Set all bytes to zero
    my_strncpy(header->name, file_name, 100);
    my_itoa_octal(st.st_mode & 0777, header->mode, 7);
    my_itoa_octal(st.st_uid, header->uid, 7);
    my_itoa_octal(st.st_gid, header->gid, 7);
    my_itoa_octal((unsigned int)st.st_size, header->size, 11);
    my_itoa_octal((unsigned int)st.st_mtime, header->mtime, 11);
    my_memset(header->checksum, ' ', 8);  // Placeholder for checksum
    header->typeflag = S_ISDIR(st.st_mode) ? '5' : '0';

    // Compute checksum
    unsigned int checksum = 0;
    unsigned char *bytes = (unsigned char *)header;
    for (int i = 0; i < sizeof(tar_header); i++) {
        checksum += bytes[i];
    }
    my_itoa_octal(checksum, header->checksum, 6);

    return 0;
}

// Write file contents to archive
int write_file_to_archive(int archive_fd, const char *file_name) {
    int file_fd = open(file_name, O_RDONLY);
    if (file_fd < 0) {
        write_error("my_tar: Cannot open file\n");
        return -1;
    }

    char buffer[BLOCK_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(file_fd, buffer, BLOCK_SIZE)) > 0) {
        if (write(archive_fd, buffer, bytes_read) != bytes_read) {
            write_error("my_tar: Error writing file to archive\n");
            close(file_fd);
            return -1;
        }
    }

    close(file_fd);

    // Write padding if necessary
    if (bytes_read == 0) {
        my_memset(buffer, 0, BLOCK_SIZE);
        write(archive_fd, buffer, BLOCK_SIZE);
    }

    return 0;
}

// List archive contents
int list_archive_contents(int archive_fd) {
    tar_header header;
    while (read(archive_fd, &header, sizeof(tar_header)) > 0) {
        if (header.name[0] == '\0') {
            break;
        }
        write(1, header.name, my_strlen(header.name));
        write(1, "\n", 1);

        unsigned int file_size = my_strtol_octal(header.size);

        // Skip file contents
        lseek(archive_fd, (file_size + BLOCK_SIZE - 1) / BLOCK_SIZE * BLOCK_SIZE, SEEK_CUR);
    }

    return 0;
}

// Extract files from archive
int extract_archive(int archive_fd) {
    tar_header header;
    while (read(archive_fd, &header, sizeof(tar_header)) > 0) {
        if (header.name[0] == '\0') {
            break;
        }

        unsigned int file_size = my_strtol_octal(header.size);

        int file_fd = open(header.name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (file_fd < 0) {
            write_error("my_tar: Cannot create file\n");
            return -1;
        }

        char buffer[BLOCK_SIZE];
        ssize_t bytes_left = file_size;
        while (bytes_left > 0) {
            ssize_t bytes_to_read = bytes_left < BLOCK_SIZE ? bytes_left : BLOCK_SIZE;
            read(archive_fd, buffer, bytes_to_read);
            if (write(file_fd, buffer, bytes_to_read) != bytes_to_read) {
                write_error("my_tar: Error writing file\n");
                close(file_fd);
                return -1;
            }
            bytes_left -= bytes_to_read;
        }

        close(file_fd);

        // Skip padding
        lseek(archive_fd, (BLOCK_SIZE - (file_size % BLOCK_SIZE)) % BLOCK_SIZE, SEEK_CUR);
    }

    return 0;
}
// Main function with 
int main(int argc, char *argv[]) {
    if (argc < 4) {
        write_error("Usage: my_tar -c|-r|-t|-u|-x -f archive_name [file...]\n");
        return 1;
    }

    char *mode = argv[1];
    char *archive_name = argv[3];
    int archive_fd;

    if (my_strcmp(mode, "-c") == 0) {
        archive_fd = open(archive_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        if (archive_fd < 0) {
            write_error("my_tar: Cannot create archive\n");
            return 1;
        }
        for (int i = 4; i < argc; i++) {
            tar_header header;
            if (create_tar_header(argv[i], &header) == 0) {
                write(archive_fd, &header, sizeof(tar_header));
                write_file_to_archive(archive_fd, argv[i]);
            }
        }
        close(archive_fd);
    } else if (my_strcmp(mode, "-t") == 0) {
        archive_fd = open(archive_name, O_RDONLY);
        if (archive_fd < 0) {
            write_error("my_tar: Cannot open archive\n");
            return 1;
        }
        list_archive_contents(archive_fd);
        close(archive_fd);
    } else if (my_strcmp(mode, "-x") == 0) {
        archive_fd = open(archive_name, O_RDONLY);
        if (archive_fd < 0) {
            write_error("my_tar: Cannot open archive\n");
            return 1;
        }
        extract_archive(archive_fd);
        close(archive_fd);
    } else {
        write_error("my_tar: Unknown option\n");
        return 1;
    }

    return 0;
}
