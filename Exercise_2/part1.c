// Azriel Erenkrantz - 315336719 | Ofek Avan Danan - 211824727

#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void writeToEndOfFile(const char *message, int loop) {
  // Open the file in append mode ("a") to avoid conflicts with file pointers
  FILE *file = fopen("output.txt", "a");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Write the message at the end of the file
  for (int i = 0; i < loop; i++) {
    fprintf(file, "%s\n", message);
  }

  // Close the file
  fclose(file);
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>\n", argv[0]);
    return 1;
  }

  // Create a file initially
  int newfd = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644);
  if (newfd < 0) {
    perror("Error creating file");
    exit(EXIT_FAILURE);
  }
  close(newfd); // Close the file descriptor after creation

  int status;

  // Store command-line arguments
  int loop = atoi(argv[4]);

  // Fork the first child process
  pid_t pid1 = fork();

  if (pid1 < 0) {
    perror("fork failed for child1");
    exit(EXIT_FAILURE);
  } else if (pid1 == 0) {
    // First child process writes to the file
    writeToEndOfFile(argv[2], loop);
    exit(EXIT_SUCCESS);
  }
  pid_t pid2 = fork();
  sleep(1);

  // Fork the second child process
  if (pid2 < 0) {
    perror("fork failed for child2");
    exit(EXIT_FAILURE);
  } else if (pid2 == 0) {
    // Wait for the first child to finish
    waitpid(pid1, &status, 0);

    // Second child process writes to the file
    writeToEndOfFile(argv[3], loop);
    exit(EXIT_SUCCESS);
  }

  // Parent process waits for both child processes to complete
  waitpid(pid1, &status, 0);
  waitpid(pid2, &status, 0);

  // Parent process writes to the file
  writeToEndOfFile(argv[1], loop);

  printf("File 'output.txt' has been created and written successfully.\n");
  return 0;
}