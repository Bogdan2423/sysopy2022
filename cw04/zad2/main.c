#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h> 

void au(int sig_no) {
    printf("Handling signal %d.\n", sig_no);
    return;
}

void au1(int sig_no, siginfo_t * info, void * context){
    printf("Handling signal number %d sent from process %d\n", info->si_signo, info->si_pid);
    printf("Error number: %d\nSignal code: %d\nSending process uid: %d\n", info->si_errno, info->si_code,info->si_uid);
    return;
}

int main(int argc, char *argv[]) {
    struct sigaction act;
    act.sa_handler = au;
    act.sa_sigaction = au1;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);

    printf("Sending signal SIGUSR1...\n");
    raise(SIGUSR1); 

    return 0;
}
