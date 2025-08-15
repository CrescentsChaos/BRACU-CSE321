#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>
#define TOTAL_STUDENTS 10
#define WAITING_CHAIRS 3
sem_t customers;
sem_t done_sem[TOTAL_STUDENTS];
pthread_mutex_t mtx=PTHREAD_MUTEX_INITIALIZER;
int q[WAITING_CHAIRS], q_head=0, q_tail=0, q_count=0;
int waiting_count=0, students_enqueued=0, served_count=0, terminate_tutor=0;
void enqueue(int id) {
    q[q_tail]=id;
    q_tail=(q_tail+1) % WAITING_CHAIRS;
    q_count++;
}
int dequeue() {
    int id=q[q_head];
    q_head=(q_head+1) % WAITING_CHAIRS;
    q_count--;
    return id;
}
void *tutor_thread(void *arg) {
    (void)arg;
    while (1) {
        sem_wait(&customers);
        pthread_mutex_lock(&mtx);
        if (terminate_tutor && q_count == 0 && served_count == TOTAL_STUDENTS) {
            pthread_mutex_unlock(&mtx);
            break;
        }
        if (q_count == 0) {
            pthread_mutex_unlock(&mtx);
            continue;
        }
        int sid=dequeue();
        waiting_count--;
        printf("A waiting student started getting consultation\n");
        printf("Number of students now waiting: %d\n", waiting_count);
        pthread_mutex_unlock(&mtx);
        printf("ST giving consultation\nStudent %d is getting consultation\n", sid);
        usleep(200000);
        sem_post(&done_sem[sid-1]);
    }
    return NULL;
}
void *student_thread(void *arg) {
    int id=*(int *)arg;
    free(arg);

    usleep(100000+rand() % 300000);

    pthread_mutex_lock(&mtx);
    if (waiting_count == WAITING_CHAIRS) {
        printf("No chairs remaining. Student %d leaving...\n", id);
        served_count++;
        pthread_mutex_unlock(&mtx);
        return NULL;
    }
    waiting_count++;
    enqueue(id);
    students_enqueued++;
    printf("Student %d started waiting for consultation\n", id);
    sem_post(&customers);
    pthread_mutex_unlock(&mtx);
    sem_wait(&done_sem[id-1]);
    pthread_mutex_lock(&mtx);
    served_count++;
    printf("Student %d finished getting consultation and left\nNumber of served students: %d\n", id, served_count);
    pthread_mutex_unlock(&mtx);
    return NULL;
}
int main() {
    srand(time(NULL));
    sem_init(&customers, 0, 0);
    for (int i=0; i < TOTAL_STUDENTS; i++)
        sem_init(&done_sem[i], 0, 0);
    pthread_t tutor;
    pthread_t students[TOTAL_STUDENTS];
    pthread_create(&tutor, NULL, tutor_thread, NULL);
    for (int i=0; i < TOTAL_STUDENTS; i++) {
        int *pid=malloc(sizeof(int));
        *pid=i;
        pthread_create(&students[i], NULL, student_thread, pid);
    }
    for (int i=0; i < TOTAL_STUDENTS; i++)
        pthread_join(students[i], NULL);
    pthread_mutex_lock(&mtx);
    terminate_tutor=1;
    sem_post(&customers);
    pthread_mutex_unlock(&mtx);
    pthread_join(tutor, NULL);
    sem_destroy(&customers);
    for (int i=0; i < TOTAL_STUDENTS; i++)
        sem_destroy(&done_sem[i]);
    pthread_mutex_destroy(&mtx);
    return 0;
}
