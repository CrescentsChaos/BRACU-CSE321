#include <stdio.h>
#include <stdlib.h>

int compare_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

int main(int argc, char *argv[]) {
    if (argc <= 1) return 1;

    int n = argc - 1;
    int *nums = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) nums[i] = atoi(argv[i + 1]);

    qsort(nums, n, sizeof(int), compare_desc);

    for (int i = 0; i < n; i++) printf("%d ", nums[i]);
    printf("\n");

    free(nums);
}
