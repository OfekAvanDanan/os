#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Global variables
char *history[1000];
int historyCounter = 0;

// Print history
void printHistory() {
  for (int i = 0; i < historyCounter; i++) {
    printf("%d %s\n", i + 1, history[i]);
  }
}

void Execute(char *input) {
  if (strcmp(input, "history") == 0) {
    printHistory();
  }
}

// Main Loop
int main() {
  char input[100];

  while (1) {
    printf("$ ");

    // לבדוק אם מותר
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

    Execute(input);
  }

  return 0;
}
