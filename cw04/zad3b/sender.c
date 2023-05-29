#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>


int counter=0;
int n;
int total_n;
int max=-1;
int mode;
pid_t catcher_pid;
union sigval sig_no;

void au1(int sig_no) {
    counter++;
}

void au1_queue(int num, siginfo_t * info, void * context) {
    counter++;
    max=info->si_value.sival_int;
}

void au0(int num) {
    if (n==1){
        if (mode==0){
            signal(SIGUSR1, au1);
        }
        else if (mode==1){
            struct sigaction act1;
            act1.sa_sigaction = au1_queue;
            sigemptyset(&act1.sa_mask);
            act1.sa_flags = SA_SIGINFO;
            sigaction(SIGUSR1, &act1, NULL);
        }

        if (mode==0)
            kill(catcher_pid,SIGUSR2);
        else if (mode==1)
            sigqueue(catcher_pid,SIGUSR2,sig_no);
        else
            kill(catcher_pid,SIGRTMIN+1);
    }
    else{
        if (mode==0){
            kill(catcher_pid,SIGUSR1);
        }
        else if (mode==1){
            sigqueue(catcher_pid,SIGUSR1,sig_no);
        }
        else{
            kill(catcher_pid,SIGRTMIN);
        }
        n--;
    }
}


void au2(int sig_no) {
    printf("Sent %d signals, received %d\n",total_n,counter);
    exit(0);
}

void au2_queue(int sig_no) {
    printf("Sent %d signals, received %d\n",total_n,counter);
    printf("Catcher sent %d signals\n",max);
    exit(0);
}


int main(int argc, char *argv[]) {
    if (argc!=4){
		printf("Wrong parameters\n");
        return 1;
    }

    catcher_pid=atoi(argv[1]);
    total_n=n=atoi(argv[2]);
    
    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);
    signal(SIGUSR1, au0);

    if (strcmp(argv[3],"kill")==0){
        mode=0;
        signal(SIGUSR2, au2);
        kill(catcher_pid,SIGUSR1);
    }

    if (strcmp(argv[3],"sigqueue")==0){
        mode=1;
        signal(SIGUSR2, au2_queue);
        sigqueue(catcher_pid,SIGUSR1,sig_no);
    }

    if (strcmp(argv[3],"sigrt")==0){
        mode=2;
        signal(SIGRTMIN, au1);
        signal(SIGRTMIN+1, au2);
        kill(catcher_pid,SIGRTMIN);
    }

    if (mode==0 || mode==1){
        sigdelset(&mask,SIGUSR2);
    }

    else{
        sigdelset(&mask,SIGRTMIN);
        sigdelset(&mask,SIGRTMIN+1);
    }

    while(1)
        sigsuspend(&mask);

    return 0;
}