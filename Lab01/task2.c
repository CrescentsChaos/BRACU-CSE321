#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    if (fork() == 0) {
        if (fork() == 0) {
            printf("I am grandchild\n");
        } else {
            wait(NULL);
            printf("I am child\n");
        }
    } else {
        wait(NULL);
        printf("I am parent\n");
    }
}
