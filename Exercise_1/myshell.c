#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// Global variables
char *history[100];
int historyCounter = 0;
char *customPaths[100];
int customPathCount = 0;

// Memory Cleanup
void freeHistory() {
  for (int i = 0; i < historyCounter; i++) {
    free(history[i]);
  }
}

// Print history
void printHistory() {
  for (int i = 0; i < historyCounter; i++) {
    printf("%d %s\n", i + 1, history[i]);
  }
}

void exitShell() {
  freeHistory();
  exit(EXIT_SUCCESS);
}

void changeDirectory(char *path) {
  if (chdir(path) != 0) {
    perror("chdir failed"); // chdir failed
  }
}

void pwd() {
  char cwd[2048];
  if (getcwd(cwd, sizeof(cwd)) != NULL) {
    printf("%s\n", cwd);
  } else {
    perror("getcwd failed"); // getcwd failed
  }
}

void forker(char *args[]) {
  pid_t pid = fork();
  if (pid < 0) {
    perror("fork failed"); // fork failed
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    // Child process
    if (execvp(args[0], args) == -1) {
      // If not found in PATH, try custom paths
      for (int i = 0; i < customPathCount; i++) {
        char fullPath[2048];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", customPaths[i], args[0]);
        if (access(fullPath, X_OK) == 0) {
          execv(fullPath, args); // Execute the script from custom path if found and executable
          perror("exec failed"); // If execv fails
          exit(EXIT_FAILURE);
        }
      }
      perror("exec failed"); // exec failed
      exit(EXIT_FAILURE);
    }
  } else {
    // Parent process
    int status;
    if (waitpid(pid, &status, 0) == -1) {
      perror("waitpid failed"); // waitpid failed
    }
  }
}

void argsSpliter(char *input, char *args[], int *argc) {
  char *currChar = input;

  while (*currChar != '\0') {
    // Skip spaces
    while (*currChar == ' ') {
      currChar++;
    }

    // Check if we reached the end of the string
    if (*currChar == '\0') {
      break;
    }

    // Save the start of the argument
    args[(*argc)++] = currChar;

    // Move to the end of the current argument
    while (*currChar != '\0' && *currChar != ' ') {
      currChar++;
    }

    // If we found a space, terminate the argument and move on
    if (*currChar == ' ') {
      *currChar = '\0';
      currChar++;
    }
  }

  // Null-terminate the args array for execvp
  args[*argc] = NULL;
}

void execute(char *input) {
  char *args[100];
  int argc = 0;

  // Split the input into arguments
  argsSpliter(input, args, &argc);

  if (argc == 0) {
    // No command entered, just return
    return;
  }

  if (strcmp(args[0], "history") == 0) {
    printHistory();
  } else if (strcmp(args[0], "exit") == 0) {
    exitShell();
  } else if (strcmp(args[0], "cd") == 0) {
    if (argc < 2) {
      printf("chdir failed: Bad address\n"); // chdir failed
    } else {
      changeDirectory(args[1]);
    }
  } else if (strcmp(args[0], "pwd") == 0) {
    pwd();
  } else {
    forker(args);
  }
}

// Main Loop
int main(int argc, char *argv[]) {
  char input[100];

  // Store custom paths passed as arguments
  for (int i = 1; i < argc; i++) {
    customPaths[customPathCount++] = argv[i];
  }

  while (1) {
    printf("$ ");

    // Clean buffer
    fflush(stdout);
    if (fgets(input, sizeof(input), stdin) == NULL) {
      perror("fgets failed"); // fgets failed
      continue;
    }

    // replace \n with \0
    input[strcspn(input, "\n")] = '\0';

    // Add to history
    if (historyCounter < 100) {
      history[historyCounter++] = strdup(input);
    }

    execute(input);
  }

  return 0;
}
