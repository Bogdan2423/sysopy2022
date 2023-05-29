#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 

int main(int argc, char *argv[]){
    int n;
    if (argc!=2){
		printf("Bledne parametry");
        exit(1);
	}
	else{
		n=atoi(argv[1]);
	}

    pid_t pid;

    for(int i=0; i<n; i++){
        pid=fork();
        if(pid==0){
            printf("process %d, pid %d\n",i,getpid());
            exit(0);
        }
    }


    return 0;
}
