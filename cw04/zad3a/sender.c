#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int counter=0;
int n;
int max=-1;
void au1(int sig_no) {
    counter++;
}

void au1_queue(int num, siginfo_t * info, void * context) {
    counter++;
    max=info->si_value.sival_int;
}

void au2(int sig_no) {
    printf("Sent %d signals, received %d\n",n,counter);
    exit(0);
}

void au2_queue(int sig_no) {
    printf("Sent %d signals, received %d\n",n,counter);
    printf("Catcher sent %d signals\n",max);
    exit(0);
}


int main(int argc, char *argv[]) {
    if (argc!=4){
		printf("Wrong parameters\n");
        return 1;
    }

    pid_t catcher_pid=atoi(argv[1]);
    n=atoi(argv[2]);
    
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);

    if (strcmp(argv[3],"kill")==0){
        signal(SIGUSR1, au1);
        signal(SIGUSR2, au2);

        for (int i=0; i<n; i++){
            kill(catcher_pid,SIGUSR1);
        }

        kill(catcher_pid,SIGUSR2);
    }

    if (strcmp(argv[3],"sigqueue")==0){
        struct sigaction act1;
        act1.sa_sigaction = au1_queue;
        sigemptyset(&act1.sa_mask);
        act1.sa_flags = SA_SIGINFO;
        sigaction(SIGUSR1, &act1, NULL);
        signal(SIGUSR2, au2_queue);

        union sigval sig_no;
        for (int i=0; i<n; i++){
            sig_no.sival_int=i;
            sigqueue(catcher_pid,SIGUSR1,sig_no);
        }

        sigqueue(catcher_pid,SIGUSR2,sig_no);
    }

    if (strcmp(argv[3],"sigrt")==0){
        signal(SIGRTMIN, au1);
        signal(SIGRTMIN+1, au2);

        for (int i=0; i<n; i++)
            kill(catcher_pid,SIGRTMIN);

        kill(catcher_pid,SIGRTMIN+1);
    }

    if (strcmp(argv[3],"kill")==0||strcmp(argv[3],"sigqueue")==0){
        sigdelset(&mask,SIGUSR2);
    }

    if (strcmp(argv[3],"sigrt")==0){
        sigdelset(&mask,SIGRTMIN);
        sigdelset(&mask,SIGRTMIN+1);
    }

    while(1)
        sigsuspend(&mask);

    return 0;
}