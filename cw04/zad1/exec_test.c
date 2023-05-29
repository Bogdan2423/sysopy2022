#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]){
    if (strcmp(argv[1],"handler")==0){
        return 0;
    }
    printf("Exec process:\n");
    if (strcmp(argv[1],"pending")==0){
        sigset_t sigset2;

        sigpending(&sigset2);
        if (sigismember(&sigset2, SIGUSR1))
            printf("Signal SIGUSR1 is pending\n");
        else
            printf("Signal SIGUSR1 is NOT pending\n");
    }
    else{
        printf("Sending signal SIGUSR1...\n");
        raise(SIGUSR1); 
    }
    return 0;
}