#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void getmails(int mode){
    FILE* mail;
    if (mode==0)
        mail=popen("echo | mail | sort -k 3","w");
    else
        mail=popen("echo | mail","w");
    pclose(mail);
}

void sendmail(char* email, char* title, char* text){
    FILE* mail;
    char * command=malloc(128*sizeof(char));
    strcat(command, "echo ");
    strcat(command, text);
    strcat(command, " | mail -s ");
    strcat(command, title);
    strcat(command, " ");
    strcat(command, email);
    mail=popen(command,"w");
    pclose(mail);
}

int main(int argc, char *argv[]){
    if (argc==2){
        if (strcmp(argv[1],"nadawca"))
            getmails(1);
        else if (strcmp(argv[1],"data"))
            getmails(0);
        else
            printf("Wrong argument");
    }

    else if (argc==4){
        sendmail(argv[1],argv[2],argv[3]);
    }

    else{
        printf("Wrong arguments");
        return -1;
    }
    return 0;
}
