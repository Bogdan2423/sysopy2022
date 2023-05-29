#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "header.h"

#define MAX_CLIENTS 128

int clients[MAX_CLIENTS];
int chat_queue;

void stopall(int id){
    packet end_packet;
    end_packet.type=STOP;
    for (int j=0; j<MAX_CLIENTS; j++)
        if (clients[j]!=0)
            msgsnd(clients[j], &end_packet, PACKET_SIZE, 0);
    msgctl(chat_queue, IPC_RMID, NULL);
    exit(0);
}

int main(int argc, char* argv[]){
    key_t key = ftok(QUEUE_NAME,'p');
    chat_queue = msgget(key, IPC_CREAT|0600);
    packet standard_packet;
    for (int i=0; i<MAX_CLIENTS; i++)
        clients[i]=0;

    int client_counter=0;
    key_t client_key;
    char list_string[1024];
    char numstring[128];

    signal(SIGINT, stopall);

    while (1){
        msgrcv(chat_queue, &standard_packet, PACKET_SIZE, -5, 0);
        switch (standard_packet.type)
        {
        case STOP:
            clients[standard_packet.id]=0;
            printf("Client %d disconnected\n", standard_packet.id);
            break;
        case LIST:
            strcpy(list_string,"Connected clients: ");
            for (int j=0; j<MAX_CLIENTS; j++){
                if (clients[j]!=0){
                    sprintf(numstring, "%d, ", j);
                    strcat(list_string, numstring);
                }
            }
            strcat(list_string,"\n");
            strcpy(standard_packet.content, list_string);
            msgsnd(clients[standard_packet.id], &standard_packet, PACKET_SIZE, 0);
            standard_packet.id=-1;
            break;
        case ALL:
            for (int j=0; j<MAX_CLIENTS; j++){
                if (clients[j]!=0){
                    msgsnd(clients[j], &standard_packet, PACKET_SIZE, 0);
                }
            }
            break;
        case ONE:
            if (clients[standard_packet.receiver_id]!=0)
                msgsnd(clients[standard_packet.receiver_id], &standard_packet, PACKET_SIZE, 0);
            break;
        case INIT:
            clients[client_counter]=msgget(standard_packet.key, 0600);
            standard_packet.key=0;
            standard_packet.id=client_counter;
            msgsnd(clients[client_counter], &standard_packet, PACKET_SIZE, 0);
            printf("Client %d connected\n", client_counter);
            standard_packet.id=-1;
            client_counter++;
            break;
        
        default:
            break;
        }
    }

    msgctl(chat_queue, IPC_RMID,NULL);
    return 0;
}

//ipcs -q