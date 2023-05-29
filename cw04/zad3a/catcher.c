#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int counter=0;
void au1(int num, siginfo_t * info, void * context) {
    counter++;
    pid_t sender_pid=info->si_pid;
}

void au2_kill(int num, siginfo_t * info, void * context) {
    pid_t sender_pid=info->si_pid;
    for (int i=0; i<counter; i++)
        kill(sender_pid, SIGUSR1);
    kill(sender_pid, SIGUSR2);
    printf("Received %d signals\n", counter);
    exit(0);
}

void au2_sigqueue(int num, siginfo_t * info, void * context) {
    pid_t sender_pid=info->si_pid;
    union sigval sig_no;
    for (int i=0; i<counter; i++){
        sig_no.sival_int=i;
        sigqueue(sender_pid, SIGUSR1, sig_no);
    }
    sigqueue(sender_pid, SIGUSR2, sig_no);
    printf("Received %d signals\n", counter);
    exit(0);
}

void au2_sigrt(int num, siginfo_t * info, void * context) {
    pid_t sender_pid=info->si_pid;
    for (int i=0; i<counter; i++)
        kill(sender_pid, SIGRTMIN);
    kill(sender_pid, SIGRTMIN+1);
    printf("Received %d signals\n", counter);
    exit(0);
}

int main(int argc, char *argv[]) {
    if (argc!=2){
		printf("Wrong parameters\n");
        return 1;
    }

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;

    if (strcmp(argv[1],"kill")==0){
        act.sa_sigaction = au2_kill;
        sigaction(SIGUSR2, &act, NULL);
    }
    if (strcmp(argv[1],"sigqueue")==0){
        act.sa_sigaction = au2_sigqueue;
        sigaction(SIGUSR2, &act, NULL);
    }
    if (strcmp(argv[1],"sigrt")==0){
        act.sa_sigaction = au2_sigrt;
        sigaction(SIGRTMIN+1, &act, NULL);
    }

    printf("Catcher pid: %d\n",getpid());


    struct sigaction act1;
    sigemptyset(&act1.sa_mask);
    act1.sa_flags = SA_SIGINFO;
    act.sa_sigaction = au1;
    sigaction(SIGUSR1, &act, NULL);
    sigaction(SIGRTMIN, &act, NULL);

    sigset_t mask;
    sigfillset(&mask);
    sigdelset(&mask,SIGUSR1);
    sigdelset(&mask,SIGUSR2);
    sigdelset(&mask,SIGRTMIN);
    sigdelset(&mask,SIGRTMIN+1);

    while(1)
        sigsuspend(&mask);

    return 0;
}