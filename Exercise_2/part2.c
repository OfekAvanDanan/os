// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void writeMessage(FILE *file, const char *message, int count) {
  for (int i = 0; i < count; i++) {
    fprintf(file, "%s\n", message); // Write message to file
    usleep((rand() % 100) * 1000);  // Random delay between 0 and 99 milliseconds
  }
}

void acquireLock(const char *lockfile) {
  while (open(lockfile, O_CREAT | O_EXCL, 0644) == -1) {
    usleep(1000); // Wait
  }
}

void releaseLock(const char *lockfile) {
  if (unlink(lockfile) == -1) {
    perror("Error releasing lock");
    exit(EXIT_FAILURE);
  }
}

void writeSafely(const char *lockfile, FILE *file, const char *message, int loop) {
  acquireLock(lockfile);
  writeMessage(file, message, loop);
  releaseLock(lockfile);
}

int main(int argc, char *argv[]) {
  if (argc <= 4) {
    fprintf(stderr, "Usage: %s <message1> <message2> ... <count>\n", argv[0]);
    return EXIT_FAILURE;
  }

  const char *lockfile = "lockfile.lock";

  // Open output file using open
  int fd = open("output2.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (fd < 0) {
    perror("Error opening output2.txt");
    return EXIT_FAILURE;
  }

  // Convert fd to FILE * using fdopen
  FILE *file = fdopen(fd, "w");
  if (!file) {
    perror("Error creating FILE * from fd");
    close(fd);
    return EXIT_FAILURE;
  }

  int loop = atoi(argv[argc - 1]);

  // Fork multiple child processes
  for (int i = 1; i < argc - 1; i++) {
    pid_t pid = fork();
    if (pid < 0) {
      perror("fork failed");
      exit(EXIT_FAILURE);
    } else if (pid == 0) {
      // Child process writes safely to the file
      writeSafely(lockfile, file, argv[i], loop);
      fclose(file); // Close FILE * in the child process
      exit(EXIT_SUCCESS);
    }
  }

  // Parent waits for all child processes
  for (int i = 1; i < argc - 1; i++) {
    wait(NULL);
  }

  // Close the FILE * in the parent process
  fclose(file);

  // Print completion message to the console
  // printf("All processes have completed writing.\n");

  return EXIT_SUCCESS;
}