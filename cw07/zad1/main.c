#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define COOK_COUNT 3
#define DELIVERY_COUNT 3

union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

typedef struct
{
    int oven[5];
    int table[5];
} pizzas;

pid_t pids[COOK_COUNT + DELIVERY_COUNT];

int semaphore_id;
int shared_memory_id;

void handle_SIGINT(int signum)
{
    for (int i = 0; i < COOK_COUNT + DELIVERY_COUNT; i++)
    {
        kill(pids[i], SIGINT);
    }

}


int main(void)
{
    signal(SIGINT, handle_SIGINT);
    key_t sem_key = ftok(getenv("HOME"), 2);
    semaphore_id = semget(sem_key, 7, IPC_CREAT | 0666);
    if (semaphore_id < 0)
    {
        printf("Cannot create semaphores set %d\n", errno);
        exit(1);
    }
    union semun arg;
    

    /*semafory:
    0-wolne miejsca w piecu
    1-indeks w piecu
    2-msc w okienku
    3-wolne miejsca na stole
    4-indeks na stole dla kucharzy
    5- indeks na stole dla dostawcow
    6-pizze na stole
    */

    arg.val = 5;
    semctl(semaphore_id, 0, SETVAL, arg);
    arg.val = 0;
    semctl(semaphore_id, 1, SETVAL, arg);
    arg.val = 1;
    semctl(semaphore_id, 2, SETVAL, arg);
    arg.val = 5;
    semctl(semaphore_id, 3, SETVAL, arg);
    arg.val = 0;
    semctl(semaphore_id, 4, SETVAL, arg);
    arg.val = 0;
    semctl(semaphore_id, 5, SETVAL, arg);
    arg.val = 0;
    semctl(semaphore_id, 6, SETVAL, arg);



    key_t shm_key = ftok(getenv("HOME"), 1);
    shared_memory_id = shmget(shm_key, sizeof(pizzas), IPC_CREAT | 0666);
    if (shared_memory_id < 0)
    {
        printf("Cannot create shared memory %d\n", errno);
        exit(1);
    }
    for (int i = 0; i < COOK_COUNT; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./cook", "cook", NULL);
            exit(0);
        }
        pids[i] = child_pid;
    }

    for (int i = 0; i < DELIVERY_COUNT; i++)
    {
        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            execlp("./deliver", "deliver", NULL);
            exit(0);
        }
        pids[i + COOK_COUNT] = child_pid;
    }

    for (int i = 0; i < COOK_COUNT + DELIVERY_COUNT; i++)
    {
        wait(NULL);
    }
    semctl(semaphore_id, 0, IPC_RMID, NULL);
    shmctl(shared_memory_id, IPC_RMID, NULL);
    return 0;
}
