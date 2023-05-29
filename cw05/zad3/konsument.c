#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int to_num(char numstr[]){
    int num=0;
    for (int i = 0; i<4; i++){
        num*=10;
        num+=(numstr[i]-'0');
    }
    return num;
}


int main(int argc, char *argv[]){
     if (argc!=4){
        printf("Usage: kosument [pipe path] [file path] [N]");
        exit(1);
    }

    FILE *f = fopen(argv[2], "wr");
    if (f == NULL)
    {
        printf("Can't open file: %s", argv[2]);
        exit(1);
    }

    FILE *pipe = fopen(argv[1], "r");
    if (pipe == NULL)
    {
        printf("Can't open pipe: %s", argv[1]);
        exit(1);
    }
    int N=atoi(argv[3]);
    char buffer[N];
    int linenum;
    char numstr[4];
    char text[1024][256];
    int pointers[1024];
    for (int i;i<1024;i++)
        pointers[i]=0;

    while (fgets(numstr,5,pipe)!=NULL)
    {
        linenum=to_num(numstr);
        fgets(buffer, N+1, pipe);
        for (int i=0;i<strlen(buffer);i++){
            text[linenum][pointers[linenum]]=buffer[i];
            pointers[linenum]++;
        }
    }
    for (int i;i<1024;i++){
        fputs(text[i],f);
        fputc('\n',f);
    }

    fclose(pipe);
    fclose(f);
    return 0;
}
