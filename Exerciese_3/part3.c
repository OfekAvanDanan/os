#include "copytree.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void print_usage(const char *prog_name) {
  fprintf(stderr, "Usage: %s [-l] [-p] <source_directory> <destination_directory>\n", prog_name);
  fprintf(stderr, "  -l: Copy symbolic links as links\n");
  fprintf(stderr, "  -p: Copy file permissions\n");
}

int main(int argc, char *argv[]) {
  int opt;
  int copy_symlinks = 0;    // Default: Do not copy symlinks as links
  int copy_permissions = 0; // Default: Do not copy permissions

  // Parse command-line options
  while ((opt = getopt(argc, argv, "lp")) != -1) {
    switch (opt) {
    case 'l':
      copy_symlinks = 1;
      break;
    case 'p':
      copy_permissions = 1;
      break;
    default:
      print_usage(argv[0]);
      return EXIT_FAILURE;
    }
  }

  // Ensure the correct number of arguments (source and destination directories)
  if (optind + 2 != argc) {
    print_usage(argv[0]);
    return EXIT_FAILURE;
  }

  const char *src_dir = argv[optind];      // Source directory
  const char *dest_dir = argv[optind + 1]; // Destination directory

  // Perform the directory copy
  printf("Copying from '%s' to '%s'...\n", src_dir, dest_dir);
  printf("Options: Copy symbolic links: %s, Copy permissions: %s\n", copy_symlinks ? "ON" : "OFF",
         copy_permissions ? "ON" : "OFF");

  copy_directory(src_dir, dest_dir, copy_symlinks, copy_permissions);

  printf("Directory copied successfully.\n");
  return EXIT_SUCCESS;
}
