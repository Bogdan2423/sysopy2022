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
#include <time.h>

int semaphore_id;
int shared_memory_id;

typedef struct sembuf sembuf;

typedef struct
{
    int oven[5];
    int table[5];
} pizzas;


int main()
{
    key_t sem_key = ftok(getenv("HOME"), 2);
    semaphore_id = semget(sem_key, 0, 0);
    if (semaphore_id < 0)
    {
        printf("Can't get semaphore %d\n", errno);
        exit(1);
    }

    key_t shm_key = ftok(getenv("HOME"), 1);
    shared_memory_id = shmget(shm_key, 0, 0);
    if (shared_memory_id < 0)
    {
        printf("Can't access shared memory %d\n", errno);
        exit(1);
    }

    sembuf *sem_operation;
    int index, pizza, pizzas_on_table;

    while (1)
    {
        sem_operation = calloc(4, sizeof(sembuf));

        sem_operation[0].sem_num = 3;
        sem_operation[0].sem_op = 1;
        sem_operation[0].sem_flg = 0;

        sem_operation[1].sem_num = 5;
        sem_operation[1].sem_op = 1;
        sem_operation[1].sem_flg = 0;

        sem_operation[2].sem_num = 6;
        sem_operation[2].sem_op = -1;
        sem_operation[2].sem_flg = 0;

        sem_operation[3].sem_num = 2;
        sem_operation[3].sem_op = -1;
        sem_operation[3].sem_flg = 0;

        semop(semaphore_id, sem_operation, 4);

        pizzas *ord = shmat(shared_memory_id, NULL, 0);

        index = (semctl(semaphore_id, 5, GETVAL, NULL) - 1) % 5;
        pizza = ord->table[index];
        pizzas_on_table = (semctl(semaphore_id, 6, GETVAL, NULL));
        printf("[%d %ld] Pobieram pizze: %d. Liczba pizz na stole: %d.\n",
            getpid(), time(NULL), pizza, pizzas_on_table);
        
        sem_operation = calloc(1, sizeof(sembuf));

        sem_operation[0].sem_num = 2;
        sem_operation[0].sem_op = 1;
        sem_operation[0].sem_flg = 0;

        semop(semaphore_id, sem_operation, 1);

        sleep(5);

        printf("[%d %ld] Dostarczam pizze: %d.\n",
            getpid(), time(NULL), pizza);

        sleep(5);
    }

    return 0;
}

