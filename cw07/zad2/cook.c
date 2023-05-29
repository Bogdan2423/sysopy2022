#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <time.h>

#define SHARED_MEMORY "/SHARED_MEMORY"
const char *SEMAPHORES[7] = {"/OVEN", "/OVEN_INDEX", "/WINDOW", "/TABLE", "/COOK_INDEX", "/DELIVERY_INDEX", "/PIZZAS_ON_TABLE"};
sem_t *semaphores[7];
int shared_memory_descriptor;
int semaphore_id;
int shared_memory_id;

typedef struct sembuf sembuf;

typedef struct
{
    int oven[5];
    int table[5];
} pizzas;

void handle_SIGINT(int signum)
{
    for (int i = 0; i < 7; i++)
    {
        sem_close(semaphores[i]);
    }
    exit(0);
}

int main(void)
{
    signal(SIGINT, handle_SIGINT);
    srand(time(NULL));

    for (int i = 0; i < 7; i++)
    {
        semaphores[i] = sem_open(SEMAPHORES[i], O_RDWR);
        if (semaphores[i] < 0)
        {
            printf("Cant open semaphore\n");
            printf("Errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
    }

    shared_memory_descriptor = shm_open(SHARED_MEMORY, O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO);
    if (shared_memory_descriptor < 0)
    {
        printf("Cant access shared memory\n");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    pizzas *ord;
    int value, index, pizzas_in_oven, pizzas_on_table, table_index;
    ord = mmap(NULL, sizeof(pizzas), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_descriptor, 0);
        if (ord == (void *)-1)
        {
            printf("Cant map shared memory\n");
            printf("Errno: %d\n", errno);
            exit(EXIT_FAILURE);
        }
    while (1)
    {
        value = rand() % 10;
        printf("[%d %ld] Przygotowuje pizze: %d\n",getpid(), time(NULL), value);
        sleep(2);


        //zmniejsz msc w piecu o 1
        sem_wait(semaphores[0]);
        sem_wait(semaphores[2]);
        sem_getvalue(semaphores[0], &pizzas_in_oven);
        sem_getvalue(semaphores[1], &index);
        sem_post(semaphores[1]);
        index = index % 5;
        pizzas_in_oven = 5 - pizzas_in_oven;
        ord->oven[index] = value;
        printf("[%d %ld] Dodalem pizze: %d. Liczba pizz w piecu: %d\n",getpid(), time(NULL), value, pizzas_in_oven);
        sem_post(semaphores[2]);
        sleep(5);

        //zwieksz msc w piecu o 1, czekaj na okienko i zmniejsz na stole o 1
        sem_post(semaphores[0]);
        sem_wait(semaphores[3]);
        sem_wait(semaphores[2]);
        sem_post(semaphores[6]);
        
        sem_getvalue(semaphores[0],&pizzas_in_oven);
        pizzas_in_oven = 5 - pizzas_in_oven;
        sem_getvalue(semaphores[6],&pizzas_on_table);
        sem_getvalue(semaphores[4],&table_index);
        table_index = table_index % 5;

        ord->table[table_index] = value;
        printf("[%d %ld] Wyjmuję pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d.\n",
            getpid(), time(NULL), value, pizzas_in_oven, pizzas_on_table);

        //zwolnij okienko i zwieksz indeks stolu
        sem_post(semaphores[4]);
        sem_post(semaphores[2]);
    }

    return 0;
}