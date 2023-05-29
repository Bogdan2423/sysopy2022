#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <dlfcn.h>
#include <stdbool.h>
#include "bibl1.h"

int main(int argc, char *argv[]){
    int i=1;
    bool allocated=false;
    void* arr;
    void *handle = dlopen("./libbibl1.so", RTLD_LAZY);
    if(!handle){printf("Blad ladowania biblioteki");}
    void* (*allocate_array)();
    allocate_array = (void* (*)())dlsym(handle,"allocate_array");
    void (*word_count)();
    word_count = (void (*)())dlsym(handle,"word_count");
    int (*save_to_array)();
    save_to_array = (int (*)())dlsym(handle,"save_to_array");
    void (*clear_index)();
    clear_index = (void (*)())dlsym(handle,"clear_index");
    if(dlerror() != NULL){printf("Blad ladowania funkcji");}
    clock_t start, end;
    struct tms start_clock, end_clock;
    start=times(&start_clock);
    clock_t clock_time;
    clock_time = clock();

    while (i<argc){
        switch (argv[i][0]){
            case 'a':
                i++;
                arr = allocate_array(strtol(argv[i],NULL,10));
                allocated=true;
                i++;
                break;
            case 'c':
                i++;
                word_count(argv[i]);
                i++;
                break;
            case 's':
                i++;
                save_to_array(arr, strtol(argv[i],NULL,10));
                i++;
                break;
            case 'r':
                i++;
                clear_index(arr, strtol(argv[i],NULL,10));
                i++;
                break;
            default:
                i++;
                break;
        }
    }
    clock_time = clock() - clock_time;
    double real_time = ((double)clock_time)/CLOCKS_PER_SEC;
    end=times(&end_clock);
    int tics=sysconf(_SC_CLK_TCK);
    double user_time = (double)(end_clock.tms_utime - start_clock.tms_utime) / tics;
    double system_time = (double)(end_clock.tms_stime - start_clock.tms_stime) / tics;
    printf("%f\t%f\t%f\n", real_time, user_time, system_time);
    dlclose(handle);
    if(allocated)
        free(arr);
    return 0;
}