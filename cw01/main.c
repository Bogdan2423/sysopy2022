#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/times.h>
#include <unistd.h>
#include <stdbool.h>
#include "bibl1.h"

int main(int argc, char *argv[]){
    int i=1;
    bool allocated=false;
    void* arr;
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
    printf("%d\n", sysconf(_SC_CLK_TCK));
    printf("%f\t%f\t%f\n", real_time, user_time, system_time);
    if(allocated)
        free(arr);
    return 0;
}