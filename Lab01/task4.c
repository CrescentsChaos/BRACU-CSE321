#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

int compare_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

void check_odd_even(int arr[], int size) {
    for (int i = 0; i < size; i++) {
        if (arr[i] % 2 == 0) printf("%d is even\n", arr[i]);
        else printf("%d is odd\n", arr[i]);
    }
}

int main(int argc, char *argv[]) {
    if (argc <= 1) return 1;

    int n = argc - 1;
    int *arr = mmap(NULL, n * sizeof(int), PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    for (int i = 0; i < n; i++) arr[i] = atoi(argv[i + 1]);

    if (fork() == 0) {
        qsort(arr, n, sizeof(int), compare_desc);
        exit(0);
    } 
    else {
        wait(NULL);
        printf("Sorted array: ");
        for (int i = 0; i < n; i++) printf("%d ", arr[i]);
        printf("\n");
        check_odd_even(arr, n);
    }

    munmap(arr, n * sizeof(int));
}
