#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
  struct stat fileStat;

  // Check the source path
  if (lstat(src, &fileStat) == -1) {
    perror("COMMAND failed");
    exit(EXIT_FAILURE);
  }

  // Handle symbolic links
  if (S_ISLNK(fileStat.st_mode)) {
    if (copy_symlinks) {
      char srcTarget[PATH_MAX];
      ssize_t len = readlink(src, srcTarget, sizeof(srcTarget) - 1);
      if (len == -1) {
        perror("COMMAND failed");
        exit(EXIT_FAILURE);
      }
      srcTarget[len] = '\0';
      if (symlink(srcTarget, dest) == -1) {
        perror("COMMAND failed");
        exit(EXIT_FAILURE);
      }
    }
    return;
  }

  // Handle regular files
  if (S_ISREG(fileStat.st_mode)) {
    int srcFd = open(src, O_RDONLY);
    if (srcFd == -1) {
      perror("COMMAND failed");
      exit(EXIT_FAILURE);
    }

    int destFd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, fileStat.st_mode & 0777);
    if (destFd == -1) {
      perror("COMMAND failed");
      close(srcFd);
      exit(EXIT_FAILURE);
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytesRead, bytesWritten;
    while ((bytesRead = read(srcFd, buffer, BUFFER_SIZE)) > 0) {
      bytesWritten = write(destFd, buffer, bytesRead);
      if (bytesWritten != bytesRead) {
        perror("COMMAND failed");
        close(srcFd);
        close(destFd);
        exit(EXIT_FAILURE);
      }
    }

    if (bytesRead == -1) {
      perror("COMMAND failed");
    }

    if (copy_permissions && chmod(dest, fileStat.st_mode) == -1) {
      perror("COMMAND failed");
      close(srcFd);
      close(destFd);
      exit(EXIT_FAILURE);
    }

    close(srcFd);
    close(destFd);
  }
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
  struct stat statBuf;

  // Check if the source is a directory
  if (stat(src, &statBuf) == -1 || !S_ISDIR(statBuf.st_mode)) {
    fprintf(stderr, "Source is not a directory: %s\n", src);
    exit(EXIT_FAILURE);
  }

  // Create the target directory with default permissions
  if (mkdir(dest, 0755) == -1 && errno != EEXIST) {
    perror("COMMAND failed");
    exit(EXIT_FAILURE);
  }

  // Add the following block to set permissions if required
  if (copy_permissions && chmod(dest, statBuf.st_mode & 0777) == -1) {
    perror("COMMAND failed");
    exit(EXIT_FAILURE);
  }

  DIR *srcDir = opendir(src);
  if (!srcDir) {
    perror("COMMAND failed");
    exit(EXIT_FAILURE);
  }

  struct dirent *entry;
  while ((entry = readdir(srcDir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char srcPath[PATH_MAX];
    char destPath[PATH_MAX];
    snprintf(srcPath, sizeof(srcPath), "%s/%s", src, entry->d_name);
    snprintf(destPath, sizeof(destPath), "%s/%s", dest, entry->d_name);

    if (lstat(srcPath, &statBuf) == -1) {
      perror("COMMAND failed");
      continue;
    }

    if (S_ISDIR(statBuf.st_mode)) {
      // Recursive call for directories
      copy_directory(srcPath, destPath, copy_symlinks, copy_permissions);
    } else {
      // Call to copy files
      copy_file(srcPath, destPath, copy_symlinks, copy_permissions);
    }
  }

  closedir(srcDir);
}
