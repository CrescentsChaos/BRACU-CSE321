#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

struct msg {
    long int type;
    char txt[100];
};

#define LOGIN_TO_OTP_TYPE 1
#define OTP_TO_LOGIN_TYPE 2
#define OTP_TO_MAIL_TYPE 3
#define MAIL_TO_LOGIN_TYPE 4

int main() {
    int msgid;
    struct msg message;
    msgid = msgget((key_t)5678, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget failed");
        exit(EXIT_FAILURE);
    }
    printf("Please enter the workspace name:\n");
    char workspace[100];
    scanf("%s", workspace);
    if (strcmp(workspace, "cse321") != 0) {
        printf("Invalid workspace name\n");
        msgctl(msgid, IPC_RMID, NULL);
        exit(0);
    }
    message.type = LOGIN_TO_OTP_TYPE;
    strcpy(message.txt, workspace);
    msgsnd(msgid, &message, sizeof(message.txt), 0);
    printf("Login sent workspace name: %s\n", message.txt);
    pid_t otp_pid = fork();
    if (otp_pid < 0) {
        perror("Fork failed");
        exit(EXIT_FAILURE);
    }
    if (otp_pid == 0) {
        msgrcv(msgid, &message, sizeof(message.txt), LOGIN_TO_OTP_TYPE, 0);
        printf("OTP generator received workspace: %s\n", message.txt);
        pid_t my_pid = getpid();
        char otp_str[100];
        sprintf(otp_str, "%d", my_pid);
        message.type = OTP_TO_LOGIN_TYPE;
        strcpy(message.txt, otp_str);
        msgsnd(msgid, &message, sizeof(message.txt), 0);
        printf("OTP generator sent OTP to login: %s\n", message.txt);
        message.type = OTP_TO_MAIL_TYPE;
        msgsnd(msgid, &message, sizeof(message.txt), 0);
        printf("OTP generator sent OTP to mail: %s\n", message.txt);
        pid_t mail_pid = fork();
        if (mail_pid < 0) {
            perror("Fork failed");
            exit(EXIT_FAILURE);
        }
        if (mail_pid == 0) {
            msgrcv(msgid, &message, sizeof(message.txt), OTP_TO_MAIL_TYPE, 0);
            printf("Mail received OTP: %s\n", message.txt);

            message.type = MAIL_TO_LOGIN_TYPE;
            msgsnd(msgid, &message, sizeof(message.txt), 0);
            printf("Mail sent OTP to login: %s\n", message.txt);

            exit(0);
        } else {
            wait(NULL); 
            exit(0); 
        }
    } else {
        wait(NULL);
        struct msg otp_from_generator, otp_from_mail;
        msgrcv(msgid, &otp_from_generator, sizeof(otp_from_generator.txt), OTP_TO_LOGIN_TYPE, 0);
        printf("Login received OTP from generator: %s\n", otp_from_generator.txt);
        msgrcv(msgid, &otp_from_mail, sizeof(otp_from_mail.txt), MAIL_TO_LOGIN_TYPE, 0);
        printf("Login received OTP from mail: %s\n", otp_from_mail.txt);
        if (strcmp(otp_from_generator.txt, otp_from_mail.txt) == 0) {
            printf("OTP Verified\n");
        } else {
            printf("OTP Incorrect\n");
        }
        msgctl(msgid, IPC_RMID, NULL);
    }
    return 0;
}
