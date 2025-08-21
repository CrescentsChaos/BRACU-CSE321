#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

struct shared {
    char sel[100];
    int b;
};

int main(void) {
    int shmid;
    struct shared *sh_ptr;
    int fd[2];
    shmid = shmget((key_t)1234, sizeof(struct shared), 0666 | IPC_CREAT);
    if (shmid == -1) { perror("shmget"); exit(EXIT_FAILURE); }
    sh_ptr = (struct shared *)shmat(shmid, NULL, 0);
    if (sh_ptr == (void *)-1) { perror("shmat"); exit(EXIT_FAILURE); }
    if (pipe(fd) == -1) { perror("pipe"); exit(EXIT_FAILURE); }
    printf("Provide Your Input from Given Options:\n");
    printf("1. Type a to Add Money\n");
    printf("2. Type w to Withdraw Money\n");
    printf("3. Type c to Check Balance\n");
    char choice;
    if (scanf(" %c", &choice) != 1) {
        fprintf(stderr, "Failed to read selection\n");
        shmdt(sh_ptr);
        shmctl(shmid, IPC_RMID, NULL);
        return EXIT_FAILURE;
    }
    sh_ptr->sel[0] = choice;
    sh_ptr->sel[1] = '\0';
    sh_ptr->b = 1000;
    printf("Your selection: %s\n", sh_ptr->sel);
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        shmdt(sh_ptr);
        shmctl(shmid, IPC_RMID, NULL);
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        close(fd[0]); 
        int amount;
        switch (sh_ptr->sel[0]) {
            case 'a':
                printf("Enter amount to be added:\n");
                if (scanf("%d", &amount) == 1 && amount > 0) {
                    sh_ptr->b += amount;
                    printf("Balance added successfully\n");
                    printf("Updated balance after addition:\n%d\n", sh_ptr->b);
                } else {
                    printf("Adding failed, Invalid amount\n");
                }
                break;
            case 'w':
                printf("Enter amount to be withdrawn:\n");
                if (scanf("%d", &amount) == 1 && amount > 0 && amount <= sh_ptr->b) {
                    sh_ptr->b -= amount;
                    printf("Balance withdrawn successfully\n");
                    printf("Updated balance after withdrawal:\n%d\n", sh_ptr->b);
                } else {
                    printf("Withdrawal failed, Invalid amount\n");
                }
                break;

            case 'c':
                printf("Your current balance is:\n%d\n", sh_ptr->b);
                break;

            default:
                printf("Invalid selection\n");
                break;
        }
        const char msg[] = "Thank you for using";
        (void)write(fd[1], msg, strlen(msg));
        close(fd[1]);
        shmdt(sh_ptr);
        _exit(0);
    } else {
        close(fd[1]); 
        wait(NULL);
        char buffer[128];
        ssize_t n = read(fd[0], buffer, sizeof(buffer) - 1);
        if (n > 0) {
            buffer[n] = '\0';
            printf("%s\n", buffer);
        }
        close(fd[0]);

        shmdt(sh_ptr);
        shmctl(shmid, IPC_RMID, NULL);
    }
    return 0;
}
