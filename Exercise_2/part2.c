#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void write_message(const char *message, int count) {
  for (int i = 0; i < count; i++) {
    printf("%s\n", message);
    usleep((rand() % 100) * 1000); // Random delay between 0 and 99 milliseconds
  }
}

int main(int argc, char *argv[]) {
  if (argc <= 4) {
    fprintf(stderr, "Usage: %s <message1> <message2> ... <count>", argv[0]);
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
