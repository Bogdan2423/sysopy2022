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

#define COOK_COUNT 3
#define DELIVERY_COUNT 3
#define SHARED_MEMORY "/SHARED_MEMORY"
const char *SEMAPHORES[7] = {"/OVEN", "/OVEN_INDEX", "/WINDOW", "/TABLE", "/COOK_INDEX", "/DELIVERY_INDEX", "/PIZZAS_ON_TABLE"};
sem_t *semaphores[7];


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

    sem_t *sem;

    sem = sem_open(SEMAPHORES[0], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 5);
    if (sem == SEM_FAILED)
    {
        printf("Cant create semaphore 0\n");
        exit(1);
    }
    sem_close(sem);
    sem = sem_open(SEMAPHORES[1], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    if (sem == SEM_FAILED)
    {
        printf("Cant create semaphore 1\n");
        exit(1);
    }
    sem_close(sem);
    sem = sem_open(SEMAPHORES[2], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 1);
    if (sem == SEM_FAILED)
    {
        printf("Cant create semaphore 2\n");
        exit(1);
    }
    sem_close(sem);
    sem = sem_open(SEMAPHORES[3], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 5);
    if (sem == SEM_FAILED)
    {
        printf("Cant create semaphore 3\n");
        exit(1);
    }
    sem_close(sem);
    sem = sem_open(SEMAPHORES[4], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    if (sem == SEM_FAILED)
    {
        printf("Cant create semaphore 4\n");
        exit(1);
    }
    sem_close(sem);
    sem = sem_open(SEMAPHORES[5], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    if (sem == SEM_FAILED)
    {
        printf("Cant create semaphore 5\n");
        exit(1);
    }
    sem_close(sem);
    sem = sem_open(SEMAPHORES[6], O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO, 0);
    if (sem == SEM_FAILED)
    {
        printf("Cant create semaphore 6\n");
        exit(1);
    }
    sem_close(sem);
    

    /*semafory:
    0-wolne miejsca w piecu
    1-indeks w piecu
    2-msc w okienku
    3-wolne miejsca na stole
    4-indeks na stole dla kucharzy
    5- indeks na stole dla dostawcow
    6-pizze na stole
    */

    int fd = shm_open(SHARED_MEMORY, O_RDWR | O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO);
    if (fd < 0)
    {
        printf("Cant create shared memory\n");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, sizeof(pizzas)) < 0)
    {
        printf("Cant create segment size\n");
        printf("Errno: %d\n", errno);
        exit(EXIT_FAILURE);
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

    for (int i = 0; i < 7; i++)
        sem_unlink(SEMAPHORES[i]);
    
    shm_unlink(SHARED_MEMORY);

    return 0;
}