#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

void au(int sig_no) {
    printf("Handling signal %d.\n", sig_no);
}

int main(int argc, char *argv[]) {
    if (argc!=2){
		printf("Wrong parameters\n");
        return 1;
	}
	else{
        if (strcmp(argv[1],"ignore")==0){
            signal(SIGUSR1, SIG_IGN);
        }
        else if (strcmp(argv[1],"handler")==0){
            struct sigaction act;
            act.sa_handler = au;
            sigemptyset(&act.sa_mask);
            act.sa_flags = 0;
            sigaction(SIGUSR1, &act, NULL);
        }
        else if (strcmp(argv[1],"mask")==0 || strcmp(argv[1],"pending")==0){
            sigset_t newmask;
            sigset_t oldmask; 
            sigemptyset(&newmask); 
            sigaddset(&newmask, SIGUSR1); 
            sigprocmask(SIG_BLOCK, &newmask, &oldmask);
        }
	}
    printf("Sending signal SIGUSR1...\n");
    raise(SIGUSR1); 
    if (strcmp(argv[1],"pending")==0){
        sigset_t sigset;

        sigpending(&sigset);
        if (sigismember(&sigset, SIGUSR1))
            printf("Signal SIGUSR1 is pending\n");
        else
            printf("Signal SIGUSR1 is NOT pending\n");
    }

    pid_t pid;
    pid=fork();
    if(pid==0){
        printf("Forked process:\n");
        if (strcmp(argv[1],"pending")==0){
            sigset_t sigset1;

            sigpending(&sigset1);
            if (sigismember(&sigset1, SIGUSR1))
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
    execvp("./exec_test",argv);

    return 0;
}