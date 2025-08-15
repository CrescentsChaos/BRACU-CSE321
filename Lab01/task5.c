#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    printf("1. Parent process ID: %d\n", getpid());

    if (fork() == 0) {
        printf("2. Child process ID: %d\n", getpid());
        for (int i = 0; i < 3; i++) {
            if (fork() == 0) {
                printf("%d. Grand Child process ID: %d\n", i + 3, getpid());
                exit(0);
            } else {
                wait(NULL);
            }
        }
        exit(0);
    } else {
        wait(NULL);
    }

    return 0;
}
