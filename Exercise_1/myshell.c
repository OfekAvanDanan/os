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

// Print history
void printHistory() {
  for (int i = 0; i < historyCounter; i++) {
    printf("%d %s\n", i + 1, history[i]);
  }
}

void exitShell() {
  printf("exit\n");
  exit(0);
}

void changeDirectory(char *path) {
  if (chdir(path) != 0) {
    perror("cd failed");
  }
}

void execute(char *input) {
  // Split the input into args
  char *args[100];
  int argc = 0; // Counter for arguments
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
    args[argc++] = currChar;

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
  args[argc] = NULL;

  // Execute the command
  // ====================
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
      printf("cd: missing argument\n");
    } else {
      changeDirectory(args[1]);
    }
  }
}

// Main Loop
int main() {
  char input[100];

  while (1) {
    printf("$ ");

    // Clean buffer
    fflush(stdout);
    if (fgets(input, sizeof(input), stdin) == NULL) {
      perror("fgets failed");
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
