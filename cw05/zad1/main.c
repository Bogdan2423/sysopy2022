#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int max_commands=50;

void execute_command(char * command){
    int n=1;
    for (int i=0; i<strlen(command); i++){
        if (command[i]=='|')
            n++;
    }

    char ** commands=malloc(n*sizeof(char*));
    for (int i=0; i<n; i++){
        commands[i]=malloc(64*sizeof(char));
    }

    char * saveptr=command;
    char * get_command=strtok_r(command, "|", &saveptr);
    int i=0;
    while (get_command!=NULL){
        commands[i]=get_command;
        get_command=strtok_r(NULL, "|", &saveptr);
        i++;
    }

    char * getarg;
    int j;
    char *** args=malloc(n*sizeof(char**));
    for (int i=0; i<n; i++){
        args[i]=malloc(16*sizeof(char*));
        saveptr=commands[i];
        getarg=strtok_r(commands[i], " ", &saveptr);
        j=0;
        while (getarg!=NULL){
            args[i][j]=malloc(64*sizeof(char));
            args[i][j]=getarg;
            getarg=strtok_r(NULL, " ", &saveptr);
            j++;
        }
        args[i][j]=NULL;
    }
    int pipes[max_commands][2];
    for (int i = 0; i < n - 1; i++)
    {

        if (pipe(pipes[i]) < 0)
        {
            fprintf(stderr, "Error when making pipe\n");
            exit(1);
        }
    }
    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            fprintf(stderr, "Error when forking\n");
            exit(1);
        }
        else if (pid == 0)
        {
            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }
            if (i + 1 < n)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }
            for (int j = 0; j < n-1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            execvp(args[i][0], args[i]);
            exit(0);
        }
    }
    for (int j = 0; j < n-1; j++)
        {
            close(pipes[j][0]);
            close(pipes[j][1]);
        }
    for (int i=0; i<n; i++){
        wait(NULL);
    }
}


int main(int argc, char *argv[]){
    if (argc!=2){
        printf("Wrong arguments\n");
        return 1;
    }

    FILE * input=fopen(argv[1],"r");
    if (input==NULL){
        printf("Couldnt open file\n");
        return 1;
    }


    int linecount=0;
    int fulllinecount=0;
    char *line=malloc(256*sizeof(char));
    fgets(line,256,input);
    while (strcmp(line,"\n")!=0){
        linecount++;
        fgets(line,256,input);
    }
    fulllinecount=linecount;
    while (fgets(line,256,input)){
        fulllinecount++;
    }
    rewind(input);
    char ** components=malloc(linecount*sizeof(char*));
    fgets(line,256,input);
    int i=0;
    int len;
    while (strcmp(line,"\n")!=0){
        components[i]=malloc(256*sizeof(char));
        len=strlen(line);
        for (int j=13; j<len-1; j++){
            components[i][j-13]=line[j];
        }
        fgets(line,256,input);
        i++;
    }
    char ** instructions=malloc((fulllinecount-linecount)*sizeof(char*));
    i=0;
    char * buffer;
    while (fgets(line,256,input)){
        instructions[i]=malloc(256*sizeof(char));
        buffer=malloc(256*sizeof(char));
        for (int i=9; i<strlen(line) ;i+=13){
            strcat(buffer,components[line[i]-'1']);
            if (i+12<strlen(line))
                strcat(buffer," | ");
        }
        strcpy(instructions[i],buffer);
        free(buffer);
        i++;
    }
    fclose(input);

    for (int i=0; i<fulllinecount-linecount; i++){
        execute_command(instructions[i]);
    }

    return 0;
}
