#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

int main(int argc, char *argv[]){
    double dx;
    int n;

    struct timespec start, stop;
    double time;
    clock_gettime( CLOCK_REALTIME, &start);


    if (argc!=3){
		printf("Bledne parametry");
        exit(1);
	}
	else{
        dx=strtod(argv[1], NULL);
		n=atoi(argv[2]);
	}

    char* filename;
    char* intstr;
    double from, to, x, sum;
    pid_t pid1;

    for(int i=0; i<n; i++){
        from=(double)i/n;
        to=((double)i+1)/n;
        pid1=fork();

        if (pid1==0) {
            FILE *f;
            filename=calloc(10,sizeof(char));
            filename[0]='w';
            intstr=calloc(3,sizeof(char));
            sprintf(intstr, "%d", i);
            strcat(filename,intstr);
            strcat(filename,".txt");
            if ((f=fopen(filename, "w"))==NULL){
                printf ("Blad przy tworzeniu pliku\n");
                exit(1);
            }
            x=from;
            sum=0;
            while(x<=to){
                sum+=dx*4.0/(x*x+1);
                x+=dx;
            }
            fprintf(f,"%.15f", sum);
            fclose(f);
            free(filename);
            free(intstr);

            return 0;
        }

    }

    for(int i=0; i<n; i++){
        wait(NULL);
    }


    char* num;
    sum=0;
    for(int i=0; i<n; i++){
        FILE *f;
        filename=calloc(10,sizeof(char));
        filename[0]='w';
        intstr=calloc(3,sizeof(char));
        sprintf(intstr, "%d", i);
        strcat(filename,intstr);
        strcat(filename,".txt");
        if ((f=fopen(filename, "r"))==NULL){
            printf ("Blad przy otwieraniu pliku\n");
            exit(1);
        }
        
        num=calloc(20,sizeof(char));
        fgets(num,20,f);
        sum+=strtod(num, NULL);
        fclose(f);
        free(filename);
        free(intstr);
        free(num);
    }

    clock_gettime( CLOCK_REALTIME, &stop);
    time = ( stop.tv_sec - start.tv_sec ) + ( (double)stop.tv_nsec - (double)start.tv_nsec )/1000000000;
	printf("Result: %.15f\n", sum);
	printf("Execution time: %lf s\n",time);


    return 0;
}