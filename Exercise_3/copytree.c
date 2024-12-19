#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void create_directories(const char *path) {
  char tmp[PATH_MAX];
  snprintf(tmp, sizeof(tmp), "%s", path);
  char *p = tmp;

  while (*p) {
    if (*p == '/') {
      *p = '\0';
      mkdir(tmp, 0755);
      *p = '/';
    }
    p++;
  }
  mkdir(tmp, 0755);
}

void copy_file(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
  struct stat fileStat;
  if (lstat(src, &fileStat) == -1) {
    perror("COMMAND failed");
    exit(EXIT_FAILURE);
  }

  char srcTarget[PATH_MAX];
  if (S_ISLNK(fileStat.st_mode)) {
    ssize_t len = readlink(src, srcTarget, sizeof(srcTarget) - 1);
    if (len == -1) {
      perror("COMMAND failed");
      exit(EXIT_FAILURE);
    }
    srcTarget[len] = '\0';
  } else {
    strcpy(srcTarget, src);
  }

  if (copy_symlinks) {
    if (symlink(srcTarget, dest) == -1) {
      perror("COMMAND failed");
      exit(EXIT_FAILURE);
    }
    return;
  }

  int srcFd = open(srcTarget, O_RDONLY);
  if (srcFd == -1) {
    perror("COMMAND failed");
    exit(EXIT_FAILURE);
  }

  create_directories(dest);

  int destFd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (destFd == -1) {
    perror("COMMAND failed");
    close(srcFd);
    exit(EXIT_FAILURE);
  }

  char buffer[BUFFER_SIZE];
  ssize_t bytes_read, bytes_written;
  while ((bytes_read = read(srcFd, buffer, BUFFER_SIZE)) > 0) {
    bytes_written = write(destFd, buffer, bytes_read);
    if (bytes_written != bytes_read) {
      perror("COMMAND failed");
      close(srcFd);
      close(destFd);
      exit(EXIT_FAILURE);
    }
  }

  if (copy_permissions) {
    if (chmod(dest, fileStat.st_mode) == -1) {
      perror("COMMAND failed");
      close(srcFd);
      close(destFd);
      exit(EXIT_FAILURE);
    }
  }

  close(srcFd);
  close(destFd);
}

void copy_directory(const char *src, const char *dest, int copy_symlinks, int copy_permissions) {
  DIR *srcDir = opendir(src);
  if (srcDir == NULL) {
    perror("COMMAND failed");
    exit(EXIT_FAILURE);
  }

  create_directories(dest);

  struct dirent *entry;
  while ((entry = readdir(srcDir)) != NULL) {
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    char srcPath[PATH_MAX];
    char destPath[PATH_MAX];
    snprintf(srcPath, sizeof(srcPath), "%s/%s", src, entry->d_name);
    snprintf(destPath, sizeof(destPath), "%s/%s", dest, entry->d_name);

    struct stat stat_buf;
    if (lstat(srcPath, &stat_buf) == -1) {
      perror("lstat failed");
      continue;
    }

    if (S_ISDIR(stat_buf.st_mode)) {
      copy_directory(srcPath, destPath, copy_symlinks, copy_permissions);
    } else {
      copy_file(srcPath, destPath, copy_symlinks, copy_permissions);
    }
  }

  closedir(srcDir);
}
