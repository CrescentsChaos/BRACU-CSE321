#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    long long *fib;
    int *idx;
    int size, count;
} SearchArgs;

void *gen_fib(void *arg) {
    int n=*(int *)arg;
    long long *f=malloc((n+1) * sizeof(long long));
    if (!f) pthread_exit(NULL);
    f[0]=0;
    if (n > 0) f[1]=1;
    for (int i=2; i <= n; i++) f[i]=f[i - 1]+f[i - 2];
    pthread_exit(f);
}

void *search_fib(void *arg) {
    SearchArgs *a=arg;
    for (int i=0; i < a->count; i++)
        if (a->idx[i] >= 0 && a->idx[i] <= a->size)
            printf("result of search #%d = %lld\n", i+1, a->fib[a->idx[i]]);
        else
            printf("result of search #%d = -1\n", i+1);
    pthread_exit(NULL);
}

int main() {
    pthread_t t1, t2;
    int n, q;
    long long *fib=NULL;
    printf("Enter the term of fibonacci sequence:");
    scanf("%d", &n);
    if (n < 0 || n > 100) return 1;
    pthread_create(&t1, NULL, gen_fib, &n);
    pthread_join(t1, (void **)&fib);
    for (int i=0; i <= n; i++) printf("a[%d]=%lld\n", i, fib[i]);
    printf("Number of searches: ");
    scanf("%d", &q);
    if (q <= 0) { free(fib); return 1; }
    int *idx=malloc(q * sizeof(int));
    for (int i=0; i < q; i++) {
        printf("Enter search %d: ", i+1);
        scanf("%d", &idx[i]);
    }
    SearchArgs args={fib, idx, n, q};
    pthread_create(&t2, NULL, search_fib, &args);
    pthread_join(t2, NULL);
    free(fib);
    free(idx);
    return 0;
}
