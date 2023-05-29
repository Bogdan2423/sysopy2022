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


int main()
{
    signal(SIGINT, handle_SIGINT);
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
    int index, pizza, pizzas_on_table;
    ord = mmap(NULL, sizeof(pizzas), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_descriptor, 0);
        if (ord == (void *)-1)
        {
            printf("Cant map shared memory\n");
            printf("Errno: %d\n", errno);
            exit(1);
        }
    while (1)
    {

        sem_wait(semaphores[6]);
        sem_wait(semaphores[2]);
        sem_post(semaphores[5]);
        sem_post(semaphores[3]);

        sem_getvalue(semaphores[5], &index);
        index = (index - 1) % 5;
        pizza = ord->table[index];
        sem_getvalue(semaphores[6], &pizzas_on_table);
        printf("[%d %ld] Pobieram pizze: %d. Liczba pizz na stole: %d.\n",
            getpid(), time(NULL), pizza, pizzas_on_table);
        
        sem_post(semaphores[2]);

        sleep(5);

        printf("[%d %ld] Dostarczam pizze: %d.\n",
            getpid(), time(NULL), pizza);

        sleep(5);
    }

    return 0;
}

