#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

int main(int argc, char *argv[]){
    if (argc!=5){
        printf("Usage: producent [pipe path] [line number] [file path] [N]");
        exit(1);
    }

    FILE * f;
    f=fopen(argv[3],"r");
    if (f == NULL)
    {
        printf("Can't open file %s", argv[2]);
        exit(1);
    }

    int pipe = open(argv[1], O_WRONLY);
    if (pipe < 0)
    {
        printf("Can't open pipe: %s", argv[1]);
        exit(1);
    }
    int N=atoi(argv[4]);
    char buffer[N];
    srand(time(NULL));
    int my_number=atoi(argv[2]);
    char numstring[4];
    for (int i=0; i<4; i++)
        numstring[i]='0';
    int k=3;
    while (my_number>0){
        numstring[k]=my_number%10+'0';
        my_number=my_number/10;
        k--;
    }
    while (fgets(buffer, N+1, f) != NULL)
    {
        char message[N + 4];
        sprintf(message, "%s%s", numstring, buffer);
        write(pipe, message, strlen(message));
        sleep(rand() % 3);
    }
    close(pipe);
    fclose(f);

    return 0;
}
