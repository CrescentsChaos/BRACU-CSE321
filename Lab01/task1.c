#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        return 1;
    }
    FILE *fp = fopen(argv[1], "w");
    if (!fp) {
        return 1;
    }

    char str[256];
    printf("Enter strings to write to the file.\n");
    printf("Type '-1' and press Enter to stop.\n");
    while (1) {
        printf("> ");
        fgets(str, 256, stdin);
        str[strcspn(str, "\n")] = 0;
        if (strcmp(str, "-1") == 0) {
            break;
        }
        fprintf(fp, "%s\n", str);
    }
    fclose(fp);
}
