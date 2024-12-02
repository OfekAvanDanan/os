

#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

// Static variables
static int counter = 2;
static int max = 0;
static int loop = 0;
static char **strings; // Corrected: Changed the pointer type to match argv

void writeToEndOfFile(const char *message) {
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
  fprintf(file, "%s\n", message);

  // Close the file
  fclose(file);
}

int recursiveForking() {
  if (counter >= max - 1) {
    return 0;
  }
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed"); // fork failed
    exit(EXIT_FAILURE);
  } else if (pid == 0) {

    for (int i = 1; i < loop; i++) {
      writeToEndOfFile(strings[counter]);
    }
    counter++;

    recursiveForking();
    exit(EXIT_SUCCESS);
  } else {
    int status;
    waitpid(pid, &status, 0); // Wait for the child process to finish
  }
  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 5) {
    fprintf(stderr, "Usage: %s <parent_message> <child1_message> <child2_message> <count>", argv[0]);
    return 1;
  }

  // Store command-line arguments
  max = argc;
  strings = argv;
  loop = parsInt(argv[argc - 1]);

  // Create a file initially
  FILE *file = fopen("output.txt", "w");
  if (file == NULL) {
    perror("Error creating the file");
    return EXIT_FAILURE; // Exit if file creation fails
  }
  fclose(file); // Close the file immediately after creation

  // Call the function to handle file operations

  printf("File 'output.txt' has been created and written successfully.\n");
  return 0; // Exit the program successfully
}
