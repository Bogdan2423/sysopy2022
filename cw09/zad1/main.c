#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond   = PTHREAD_COND_INITIALIZER;
pthread_t thread_ids[20];

int elves=0;
int reindeers=0;
pthread_t workshop[10];
int elf_index=0;
int santa_index=0;
int deliveries=0;
int solving = 0;

void* santa(void* nothing) {
    while(1){
        pthread_mutex_lock (&mutex);
        if (elves<3 && reindeers<9){
            printf("Mikolaj: zasypiam\n");
            pthread_cond_wait(&cond, &mutex);
            printf("Mikolaj: budze sie\n");
        }
        if (reindeers >= 9){
            printf("Mikolaj: dostarczam zabawki %d raz\n", deliveries+1);
            sleep(2+rand()%3);
            reindeers=0;
            deliveries++;
            pthread_cond_broadcast(&cond);
        }
        if (deliveries == 3){
            for (int i = 1; i<20; i++)
                pthread_cancel(thread_ids[i]);
            exit(0);
        }
        if (elves >= 3){
            printf("Mikolaj: rozwiazuje problemy elfow %ld %ld %ld\n",
            workshop[santa_index], workshop[(santa_index+1)%10], workshop[(santa_index+2)%10]);
            solving = 3;
            pthread_cond_broadcast(&cond);
            while (solving>0){
                pthread_cond_wait(&cond, &mutex);
            }
            sleep(1+rand()%2);
            santa_index+=3;
            santa_index=santa_index%10;
            elves=0;
            pthread_cond_broadcast(&cond);
        } 

        pthread_mutex_unlock (&mutex);
    }
}

void* reindeer(void* nothing) {
    pthread_t id = pthread_self();
    while(1){
        sleep(5+rand()%6);

        pthread_mutex_lock (&mutex);
        reindeers++;
        if (reindeers<9){
            printf("Renifer: czeka %d reniferow na Mikolaja %ld\n", reindeers, id);
            pthread_cond_wait(&cond, &mutex);
        }
        else{
            printf("Renifer: Wybudzam Mikolaja %ld\n", id);
            pthread_cond_broadcast(&cond);
        }

        while (reindeers>=9){
            pthread_cond_wait(&cond, &mutex);
        }
        pthread_mutex_unlock (&mutex);
    }
}

void* elf(void* nothing) {
    pthread_t id = pthread_self();
    while(1){
        sleep(2+rand()%4);

        pthread_mutex_lock (&mutex);
        while (elves>=3){
            pthread_cond_wait(&cond, &mutex);
        }
        elves++;
        workshop[elf_index] = id;
        elf_index++;
        
        if (elves<3){
            printf("Elf: czeka %d elfow na Mikolaja %ld\n", elves, id);
            pthread_cond_wait(&cond, &mutex);
        }
        else{
            printf("Elf: Wybudzam Mikolaja %ld\n", id);
            pthread_cond_broadcast(&cond);
        }

        while(solving==0){
            pthread_cond_wait(&cond, &mutex);
        }
        solving--;
        if (solving==0){
            pthread_cond_broadcast(&cond);
        }
        printf("Elf: Mikolaj rozwiazuje problem %ld\n", id);

        pthread_mutex_unlock (&mutex);
    }
}

int main(void){
    srand(time(NULL));
    pthread_create (&thread_ids[0], NULL, &santa, NULL);
    for (int i = 0; i<9; i++){
        pthread_create (&thread_ids[1+i], NULL, &reindeer, NULL);
    }
    for (int i = 0; i<10; i++){
        pthread_create (&thread_ids[10+i], NULL, &elf, NULL);
    }

    for (int i = 0; i<20; i++){
        pthread_join(thread_ids[i], NULL);
    }

    return 0;
}