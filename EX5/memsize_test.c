#include "kernel/types.h"
#include "user/user.h"
int main() {
    printf("I am using %d bytes.\n", memsize());
    char *boring = (char *)malloc(20000);
    printf("Now, I am using %d bytes.\n", memsize());
    free(boring);
    printf("Now after releasing memory, I am using %d bytes.\n", memsize());
    return 0;
}
