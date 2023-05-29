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
	struct sigaction act2;
    act2.sa_handler = au;
    sigemptyset(&act2.sa_mask);
    act2.sa_flags = SA_NOCLDWAIT;
    sigaction(SIGCHLD, &act2, NULL);

	printf("Creating child process...\n");
    pid_t pid=fork();
    if (pid==0){
    printf("Child process exiting...\n");
        return 0;
    }
    sleep(2);
    printf("Waiting for child process...\n");
    pid_t child_pid=wait(NULL);
    if (errno==ECHILD)
        printf("Error: no child processes\n");
    else {printf("Child pid: %d\n",child_pid);}
    return 0;
}
