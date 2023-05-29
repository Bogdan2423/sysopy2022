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
    printf("Handling SIGCHLD signal %d.\n", sig_no);
    return;
}

int main(int argc, char *argv[]) {
    struct sigaction act1;
    act1.sa_handler = au;
    sigemptyset(&act1.sa_mask);
    act1.sa_flags = SA_NOCLDSTOP;
    sigaction(SIGCHLD, &act1, NULL);

	printf("Creating child process...\n");
    pid_t pid=fork();
    if (pid!=0){
    	printf("Stopping child process...\n");
        kill(pid,SIGSTOP);
        sleep(2);
        printf("Resuming child process...\n");
        kill(pid,SIGCONT);
    }
    else{
        sleep(2);
        printf("Child exiting\n");
        exit(0);
    }
    sleep(3);
}
