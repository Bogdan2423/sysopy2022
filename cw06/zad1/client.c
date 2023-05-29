#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "header.h"

int my_queue, main_queue, my_id;
pid_t child_pid;

void send_stop(int id){
    printf("Disconnecting...\n");
    packet stop_packet;
    stop_packet.type=STOP;
    stop_packet.id=my_id;
    msgsnd(main_queue, &stop_packet, PACKET_SIZE, 0);
    msgctl(my_queue, IPC_RMID, NULL);
    kill(child_pid, SIGKILL);
    exit(0);
}

int main(int argc, char* argv[]){
    key_t main_key = ftok(QUEUE_NAME,'p');
    main_queue = msgget(main_key, 0600);

    key_t my_key = ftok(getenv("HOME"), getpid());
    my_queue = msgget(my_key, IPC_CREAT|0600);
    packet init_packet;
    init_packet.type = INIT;
    init_packet.key = my_key;
    msgsnd(main_queue, &init_packet, PACKET_SIZE, 0);
    msgrcv(my_queue, &init_packet, PACKET_SIZE, INIT, 0);
    my_id=init_packet.id;
    printf("Connected with id %d\n",my_id);
    time_t t;
    struct tm tm;
    char timestr[128];

    packet out_packet, in_packet;
    out_packet.id=my_id;
    child_pid=fork();
    if (child_pid==0){
        while(1){
            char input[MAX_SIZE];
            fgets(input,MAX_SIZE,stdin);
            if (strcmp(input,"STOP\n")==0){
                kill(getppid(),SIGINT);
            }
            if (strcmp(input,"LIST\n")==0){
                out_packet.type=LIST;
                msgsnd(main_queue, &out_packet, PACKET_SIZE, 0);
            }
            if (strcmp(input,"2ALL\n")==0){
                out_packet.type=ALL;
                printf("Message: ");
                fgets(input, MAX_SIZE, stdin);
                t = time(NULL);
                tm = *localtime(&t);
                sprintf(timestr, "%d-%02d-%02d %02d:%02d:%02d: ",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                strcpy(out_packet.content, timestr);
                strcat(out_packet.content, input);
                msgsnd(main_queue, &out_packet, PACKET_SIZE, 0);
            }
            if (strcmp(input,"2ONE\n")==0){
                out_packet.type=ONE;
                printf("Client id: ");
                fgets(input, MAX_SIZE, stdin);
                out_packet.receiver_id=atoi(input);
                printf("Message: ");
                fgets(input, MAX_SIZE, stdin);
                t = time(NULL);
                tm = *localtime(&t);
                sprintf(timestr, "%d-%02d-%02d %02d:%02d:%02d: ",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                strcpy(out_packet.content, timestr);
                strcat(out_packet.content, input);
                msgsnd(main_queue, &out_packet, PACKET_SIZE, 0);
            }
        }
        return 0;
    }
    else{
        signal(SIGINT, send_stop);
        while(1){
            msgrcv(my_queue, &in_packet, PACKET_SIZE, -5, 0);
            if(in_packet.type==STOP)
                raise(SIGINT);
            else if(in_packet.type==LIST)
                printf("%s\n", in_packet.content);
            else{
                printf("Client %d on %s\n",
                in_packet.id,
                in_packet.content);
            }
        }
    }
    return 0;
}

//ipcs -q
