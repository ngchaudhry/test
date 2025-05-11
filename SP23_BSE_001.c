// added a useless comments
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <time.h>

#define NUM_PHILOSOPHERS 5

int semid;
//indented comments as said in the assignment 

//Below are all the System V semaphore operations
void down(int sem_num) {
    struct sembuf s = {sem_num, -1, 0};
    semop(semid, &s, 1);
}

void up(int sem_num) {
    struct sembuf s = {sem_num, 1, 0};
    semop(semid, &s, 1);
}

void* philosopher(void* arg) {
    int id = *(int*)arg;
    srand(time(NULL) + id);  //this id over here is like a unique seed for each thread

    while (1) {
        printf("philosopher # %d wants to eat\n", id);

        //here the philosophers pick up chopsticks (left then right)
        down(id);                          // left chopstick
        down((id + 1) % NUM_PHILOSOPHERS); // right chopstick

        printf("philosopher # %d going to eat\n", id);
        sleep(rand() % 3 + 1);  // eating time is 1-3 sec

        printf("philosopher # %d done eating, now going to think\n", id);

        //here you put down chopsticks
        up(id);                          // left
        up((id + 1) % NUM_PHILOSOPHERS); // right

        sleep(rand() % 3 + 1);  // thinking time 1-3 sec
    }

    return NULL;
}

int main() {
    pthread_t threads[NUM_PHILOSOPHERS];
    int ids[NUM_PHILOSOPHERS];

    //here we are creating semaphore set with NUM_PHILOSOPHERS semaphores
    semid = semget(IPC_PRIVATE, NUM_PHILOSOPHERS, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget failed");
        exit(1);
    }

    //and here we initialize all semaphores to 1 (each chopstick is available)
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        semctl(semid, i, SETVAL, 1);
    }

    //here we are creating philosopher threads
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, philosopher, &ids[i]);
    }

    //here we join threads and this will go on and on in an infinite never ending loop
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
        pthread_join(threads[i], NULL);
    }

    //here we remove semaphores but this is unreachable in this code due to infinite loop
    semctl(semid, 0, IPC_RMID, 0);

    return 0;
}

