

#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void writeToEndOfFile(const char *message, int loop) {
  // Open the file in read-write mode ("r+")
  FILE *file = fopen("output.txt", "r+");
  if (file == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }

  // Move the file pointer to the end of the file
  if (fseek(file, 0, SEEK_END) != 0) {
    perror("Error seeking to end of file");
    fclose(file);
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
    fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>", argv[0]);
    return 1;
  }
  int newfd;
  // Create a file initially
  if ((newfd = open("output.txt", O_CREAT | O_TRUNC | O_WRONLY, 0644)) < 0) {
    perror(argv[1]); /* open failed */
    exit(1);
  }

  // Store command-line arguments
  int loop = atoi(argv[4]);

  // Fork from the children
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed"); // fork failed
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    writeToEndOfFile(argv[2], loop);

    pid_t pid2 = fork();
    if (pid2 < 0) {
      perror("fork failed"); // fork failed
      exit(EXIT_FAILURE);
    } else if (pid2 == 0) {
      writeToEndOfFile(argv[3], loop);
      exit(EXIT_SUCCESS);
    } else {
      int status;
      waitpid(pid, &status, 0); // Wait for the child process to finish
      exit(EXIT_SUCCESS);
    }

  } else {
    int status;
    waitpid(pid, &status, 0); // Wait for the child process to finish
    writeToEndOfFile(argv[1], loop);
  }

  printf("File 'output.txt' has been created and written successfully.\n");
  return 0; // Exit the program successfully
}
