#include <mqueue.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include "header.h"

#define MAX_CLIENTS 128

mqd_t clients[MAX_CLIENTS];
mqd_t chat_queue;
char content[MAX_SIZE];

void stopall(int id){
    sprintf(content,"%d",STOP);
    for (int j=0; j<MAX_CLIENTS; j++)
        if (clients[j]!=0)
            mq_send(clients[j], content, MAX_SIZE, 0);
    mq_close(chat_queue);
    mq_unlink(QUEUE_NAME);
    exit(0);
}

int get_sender_id(const char* content){
    char id[3];
    for (int i=0; i<3; i++)
        id[i]=content[i+1];
    return atoi(id);
}

int get_receiver_id(const char* content){
    char id[3];
    for (int i=0; i<3; i++)
        id[i]=content[i+4];
    return atoi(id);
}

void get_content(char* content, char* output){
    char stripped_content[MAX_SIZE-7];
    for (int i=0; i<MAX_SIZE-7; i++)
        output[i]=content[i+7];
}

int main(int argc, char* argv[]){
    struct mq_attr atr;

    atr.mq_flags=0;
    atr.mq_maxmsg=10;
    atr.mq_msgsize=MAX_SIZE;
    atr.mq_curmsgs=0;

    chat_queue = mq_open(QUEUE_NAME,O_CREAT|O_RDWR,0644,&atr);
    for (int i=0; i<MAX_CLIENTS; i++)
        clients[i]=0;

    int client_counter=0;
    key_t client_key;
    char numstring[128];
    int sender_id;
    int receiver_id;
    char path1[128];
    char id_str[3];

    signal(SIGINT, stopall);

    while (1){
        mq_receive(chat_queue, content, MAX_SIZE,NULL);
        sender_id=get_sender_id(content);
        receiver_id=get_receiver_id(content);
        switch (content[0]-'0')
        {
        case STOP:
            mq_close(clients[sender_id]);
            clients[sender_id]=0;
            printf("Client %d disconnected\n", sender_id);
            break;
        case LIST:
            sprintf(content,"%d",LIST);
            strcat(content,"Connected clients: ");
            for (int j=0; j<MAX_CLIENTS; j++){
                if (clients[j]!=0){
                    sprintf(numstring, "%d, ", j);
                    strcat(content, numstring);
                }
            }
            strcat(content,"\n");
            mq_send(clients[sender_id], content, MAX_SIZE, 0);
            break;
        case ALL:
            for (int j=0; j<MAX_CLIENTS; j++){
                if (clients[j]!=0){
                    mq_send(clients[j], content, MAX_SIZE, 0);
                }
            }
            break;
        case ONE:
            if (clients[receiver_id]!=0)
                mq_send(clients[receiver_id], content, MAX_SIZE, 0);
            break;
        case INIT:
            get_content(content, path1);
            clients[client_counter]=mq_open(path1,O_RDWR);
            if (client_counter<10){
                id_str[2]=client_counter+'0';
                id_str[1]='0';
                id_str[0]='0';
            }
            else if (client_counter<100){
                id_str[2]=client_counter%10+'0';
                id_str[1]=client_counter/10+'0';
                id_str[0]='0';
            }
            else{
                id_str[2]=((client_counter%100)%10)+'0';
                id_str[1]=((client_counter/10)%10)+'0';
                id_str[0]=client_counter/100+'0';
            }
            sprintf(content,"%d%s",INIT,id_str);
            mq_send(clients[client_counter], content, MAX_SIZE, 0);
            printf("Client %d connected\n", client_counter);
            client_counter++;
            break;
        
        default:
            break;
        }
    }

    return 0;
}

//ipcs -q