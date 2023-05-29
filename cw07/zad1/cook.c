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

int main(void)
{
    srand(time(NULL));

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

    pizzas *ord;
    int value, index, pizzas_in_oven, pizzas_on_table, table_index;

    while (1)
    {
            
        ord = shmat(shared_memory_id, NULL, 0);
        value = rand() % 10;
        printf("[%d %ld] Przygotowuje pizze: %d\n",getpid(), time(NULL), value);
        sleep(2);


        //zmniejsz msc w piecu o 1
        sembuf *sem_operation = calloc(2, sizeof(sembuf));

        sem_operation[0].sem_num = 0;
        sem_operation[0].sem_op = -1;
        sem_operation[0].sem_flg = 0;

        sem_operation[1].sem_num = 2;
        sem_operation[1].sem_op = -1;
        sem_operation[1].sem_flg = 0;

        semop(semaphore_id, sem_operation, 2);

        index = (semctl(semaphore_id, 1, GETVAL, NULL)) % 5;
        pizzas_in_oven = 5 - (semctl(semaphore_id, 0, GETVAL, NULL));
        ord->oven[index] = value;
        printf("[%d %ld] Dodalem pizze: %d. Liczba pizz w piecu: %d\n",getpid(), time(NULL), value, pizzas_in_oven);

        sem_operation = calloc(2, sizeof(sembuf));

        sem_operation[0].sem_num = 1;
        sem_operation[0].sem_op = 1;
        sem_operation[0].sem_flg = 0;

        sem_operation[1].sem_num = 2;
        sem_operation[1].sem_op = 1;
        sem_operation[1].sem_flg = 0;

        semop(semaphore_id, sem_operation, 2);

        sleep(5);

        //zwieksz msc w piecu o 1, czekaj na okienko i zmniejsz na stole o 1
        sem_operation = calloc(4, sizeof(sembuf));

        sem_operation[0].sem_num = 0;
        sem_operation[0].sem_op = 1;
        sem_operation[0].sem_flg = 0;

        sem_operation[1].sem_num = 2;
        sem_operation[1].sem_op = -1;
        sem_operation[1].sem_flg = 0;

        sem_operation[2].sem_num = 3;
        sem_operation[2].sem_op = -1;
        sem_operation[2].sem_flg = 0;

        sem_operation[3].sem_num = 6;
        sem_operation[3].sem_op = 1;
        sem_operation[3].sem_flg = 0;

        semop(semaphore_id, sem_operation, 4);

        pizzas_in_oven = 5 - (semctl(semaphore_id, 0, GETVAL, NULL));
        pizzas_on_table = (semctl(semaphore_id, 6, GETVAL, NULL));
        table_index = (semctl(semaphore_id, 4, GETVAL, NULL)) % 5;

        ord->table[table_index] = value;
        printf("[%d %ld] Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
            getpid(), time(NULL), value, pizzas_in_oven, pizzas_on_table);

        //zwolnij okienko i zwieksz indeks stolu
        sem_operation = calloc(2, sizeof(sembuf));

        sem_operation[0].sem_num = 2;
        sem_operation[0].sem_op = 1;
        sem_operation[0].sem_flg = 0;

        sem_operation[1].sem_num = 4;
        sem_operation[1].sem_op = 1;
        sem_operation[1].sem_flg = 0;

        semop(semaphore_id, sem_operation, 2);
    }

    return 0;
}