#include <stdio.h>
#include <unistd.h>
#include <sys/file.h>

#define FILE_NAME "process_counter.tmp"

void increment() {
    FILE *f = fopen(FILE_NAME, "r+");
    int fd = fileno(f), count;
    flock(fd, LOCK_EX);
    fscanf(f, "%d", &count);
    rewind(f);
    fprintf(f, "%d", count + 1);
    fflush(f);
    flock(fd, LOCK_UN);
    fclose(f);
}

int main() {
    pid_t original = getpid();

    FILE *f = fopen(FILE_NAME, "w");
    fprintf(f, "1");
    fclose(f);

    pid_t a = fork();
    if (a > 0) {
        increment();
    }

    pid_t b = fork();
    if (b > 0) {
        increment();
    }

    pid_t c = fork();
    if (c > 0) {
        increment();
    }

    if (a > 0 && a % 2 != 0) {
        if (fork() > 0) {
            increment();
        }
    }

    if (b > 0 && b % 2 != 0) {
        if (fork() > 0) {
            increment();
        }
    }

    if (c > 0 && c % 2 != 0) {
        if (fork() > 0) {
            increment();
        }
    }

    if (getpid() == original) {
        sleep(1);
        f = fopen(FILE_NAME, "r");
        int total;
        fscanf(f, "%d", &total);
        printf("Total processes created: %d\n", total);
        fclose(f);
        remove(FILE_NAME);
    }
}
