#include <asm-generic/fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#define PATH_MAX 1024
#define BUFFER_SIZE 1024

// Function to copy a file
void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
  struct stat fileStat;

  // Get file stats
  if (stat(src, &fileStat) == -1) {
    perror("COMMAND failed");
    exit(EXIT_FAILURE);
  }

  char srcTarget[PATH_MAX];

  // Check if the source is a symbolic link
  if (S_ISLNK(fileStat.st_mode)) {
    int len = readlink(src, srcTarget, sizeof(srcTarget) - 1);

    if (len == -1) {
      perror("COMMAND failed");
      exit(EXIT_FAILURE);
    }

    // Null-terminate the target path
    srcTarget[len] = '\0';
  } else if (S_ISREG(fileStat.st_mode)) {
    // If the file is regular, copy the file path
    strcpy(srcTarget, src);
  }

  // Start copying the file
  if (copy_symlinks) {
    if (symlink(srcTarget, dest) == -1) {
      perror("COMMAND failed");
      exit(EXIT_FAILURE);
    }
  } else {
    int src_fd, dest_fd;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read, bytes_written;

    // Open source file
    src_fd = open(srcTarget, O_RDONLY);
    if (src_fd == -1) {
      perror("COMMAND failed");
      exit(EXIT_FAILURE);
    }

    // Ensure destination directories exist
    char destDir[PATH_MAX];
    strncpy(destDir, dest, sizeof(destDir));
    char *lastSlash = strrchr(destDir, '/');
    if (lastSlash) {
      *lastSlash = '\0';
      create_directories(destDir);
    }

    // Open destination file
    dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
      perror("COMMAND failed");
      close(src_fd);
      exit(EXIT_FAILURE);
    }

    // Copy file content
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
      bytes_written = write(dest_fd, buffer, bytes_read);
      if (bytes_written != bytes_read) {
        perror("COMMAND failed");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
      }
    }

    if (bytes_read == -1) {
      perror("COMMAND failed");
    }

    // Apply permissions if copy_permissions is enabled
    if (copy_permissions) {
      if (chmod(dest, fileStat.st_mode) == -1) {
        perror("COMMAND failed");
        close(src_fd);
        close(dest_fd);
        exit(EXIT_FAILURE);
      }
    }

    // Close files
    close(src_fd);
    close(dest_fd);
  }
}

void create_directory(const char *path) {
  if (mkdir(path, 0755) == -1 && errno != EEXIST) {
    perror("COMMAND failed");
  }
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {}
