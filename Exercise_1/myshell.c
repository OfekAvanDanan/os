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

void execute(char *input) {
  if (strcmp(input, "history") == 0) {
    printHistory();
  } else if (strcmp(input, "exit") == 0) {
    exitShell();
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
