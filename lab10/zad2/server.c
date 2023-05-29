#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdio.h>
#include <pthread.h>
#include "shared.h"

union addr {
    struct sockaddr_un uni;
    struct sockaddr_in web;
};
typedef struct sockaddr* sa;

struct client {
    enum slot_state { empty = 0, taken = 1} slot_state;

    char name[NAME_LEN];
    char symbol;
    struct client* enemy;

    struct game_state* game_state;
    int responding;

    union addr addr;
    int addrlen;
    int sock;
} clients[MAX_CLIENTS], *waiting_client = NULL;
typedef struct client client;

int epoll;
int connected_clients = 0;

void add_client(union addr* addr, socklen_t addrlen, int sock, char* name) {
    if (connected_clients == MAX_CLIENTS) {
        message msg = { .type = server_full };
        sendto(sock, &msg, sizeof msg, 0, (sa) addr, addrlen);
        return;
    }
    
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (strncmp(name, clients[i].name, sizeof clients->name) == 0) {
            message msg = {.type = username_taken };
            sendto(sock, &msg, sizeof msg, 0, (sa) addr, addrlen);
            return;
        }
    }
    connected_clients++;

    printf("%s connecting\n%d clients connected\n", name, connected_clients);

    int new_index;
    for (int i = 0; i < MAX_CLIENTS; i++)
        if (clients[i].slot_state == empty)
            new_index = i;

    client* client = &clients[new_index];
    memcpy(&client->addr, addr, addrlen);
    client->addrlen = addrlen;
    client->slot_state = taken;
    client->responding = 1;
    client->sock = sock;

    strcpy(client->name, name);

    if (waiting_client) {
        client->enemy = waiting_client;
        waiting_client->enemy = client;

        client->game_state = malloc(sizeof client->game_state);
        waiting_client->game_state = malloc(sizeof client->game_state);
        
        for (int i = 0; i<9; i++){
            client->game_state->board[i] = '-';
            waiting_client->game_state->board[i] = '-';
        }

        client->game_state->num_fields = 0;
        waiting_client->game_state->num_fields = 0;

        message msg = { .type = start };
        waiting_client->symbol = 'O';
        msg.content.symbol = 'O';
        strncpy(msg.content.name, client->name, NAME_LEN);
        sendto(waiting_client->sock, &msg, sizeof msg, 0, (sa) &waiting_client->addr, waiting_client->addrlen);

        client->symbol = 'X';
        msg.content.symbol = 'X';
        strncpy(msg.content.name, waiting_client->name, NAME_LEN);
        sendto(client->sock, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);

        client->game_state->move = client->symbol;
        waiting_client->game_state->move = client->symbol;
        waiting_client = NULL;
    } 
    else {
        message msg = { .type = wait };
        sendto(client->sock, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
        waiting_client = client;
    }

}

void delete_client(client* client) {
    connected_clients--;
    printf("%s disconnecting\n%d clients connected\n", client->name, connected_clients);
    client->slot_state = empty;
    if (client->enemy) {
        client->enemy->enemy = NULL;
        delete_client(client->enemy);
        client->enemy = NULL;
    }
    if (client->game_state)
        client->game_state = NULL;

    if (client == waiting_client)
        waiting_client = NULL;

    message msg = { .type = disconnected };
    sendto(client->sock, &msg, sizeof msg, 0, (sa) &client->addr, client->addrlen);
    memset(&client->addr, 0, sizeof client->addr);
    client->sock = 0;
    for (int i = 0; i<NAME_LEN; i++)
        client->name[i] = 0;
}

void get_message(client* client, message* msg) {
    if (msg->type == moved) {
        int move = msg->content.move;
        char symbol = client->symbol;
        if (client->game_state->move == symbol && client->game_state->board[move] == '-' && move >= 0 && move <= 8) {
            client->game_state->board[move] = client->symbol;
            client->game_state->move = client->enemy->symbol;
            client->game_state->num_fields++;
            client->enemy->game_state->board[move] = client->symbol;
            client->enemy->game_state->move = client->enemy->symbol; 
            client->enemy->game_state->num_fields++;
            
            message msg_state = { .type = state };
            memcpy(&msg_state.content.state, client->game_state, sizeof (struct game_state));
            sendto(client->sock, &msg_state, sizeof msg_state, 0, (sa) &client->addr, client->addrlen);
            sendto(client->enemy->sock, &msg_state, sizeof msg_state, 0, (sa) &client->enemy->addr, client->enemy->addrlen);

            int win=0;
            char* board = client->game_state->board;
            for (int i=0; i<3; i++){
                if (board[i]==symbol && board[i+3]==symbol && board[i+6]==symbol)
                    win = 1;
                if (board[3*i]==symbol && board[3*i+1]==symbol && board[3*i+2]==symbol)
                    win = 1;
            }
            if (board[0]==symbol && board[4]==symbol && board[8]==symbol)
                win = 1;
            if (board[2]==symbol && board[4]==symbol && board[6]==symbol)
                win = 1;
            if (client->game_state->num_fields==9)
                win = 1;
            if (win) {
                message msg_end = { .type = end };
                if (client->game_state->num_fields==9)
                    msg_end.content.symbol = '-';
                else
                    msg_end.content.symbol = symbol;
                sendto(client->enemy->sock, &msg_end, sizeof msg_end, 0, (sa) &client->enemy->addr, client->enemy->addrlen);
                sendto(client->sock, &msg_end, sizeof msg_end, 0, (sa) &client->addr, client->addrlen);
                delete_client(client);
            }
        }
    }

    if (msg->type == disconnected) 
        delete_client(client);

    if (msg->type == ping)
        client->responding = 1;
}

void* send_pings(void* nothing) {
    const static message msg = { .type = ping };
    while(1){
        sleep(20);
        printf("Pinging clients\n");
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].slot_state != empty) {
                if (!clients[i].responding){
                    delete_client(&clients[i]);
                    continue;
                }
                sendto(clients[i].sock, &msg, sizeof msg, 0, (sa) &clients[i].addr, clients[i].addrlen);
                clients[i].responding = 0;
            }
        }
    }
    return NULL;
}

int main(int argc, char * argv[]){
    if (argc != 3) {
        printf("Usage [port] [path]\n");
        exit(0);
    }
    int port = atoi(argv[1]);
    char* path = argv[2];

    epoll = epoll_create1(0);

    struct sockaddr_un local_addr = { .sun_family = AF_UNIX };
    strncpy(local_addr.sun_path, path, sizeof local_addr.sun_path);

    struct sockaddr_in web_addr = {
        .sin_family = AF_INET, .sin_port = htons(port),
        .sin_addr = { .s_addr = htonl(INADDR_ANY) },
    };

    unlink(path);

    int local_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    bind(local_sock, (struct sockaddr*) &local_addr, sizeof local_addr);
    struct epoll_event event = { 
        .events = EPOLLIN | EPOLLPRI, 
        .data = { .fd = local_sock } 
    };
    epoll_ctl(epoll, EPOLL_CTL_ADD, local_sock, &event);

    int web_sock = socket(AF_INET, SOCK_DGRAM, 0);
    bind(web_sock, (struct sockaddr*) &web_addr, sizeof web_addr);
    struct epoll_event event1 = { 
        .events = EPOLLIN | EPOLLPRI, 
        .data = { .fd = web_sock } 
    };
    epoll_ctl(epoll, EPOLL_CTL_ADD, web_sock, &event1);

    pthread_t ping_thread;
    pthread_create(&ping_thread, NULL, send_pings, NULL);

    struct epoll_event events[MAX_CLIENTS];
    int events_num, sock;
    message msg;
    union addr addr;
    socklen_t addrlen;
    while(1) {
        events_num = epoll_wait(epoll, events, MAX_CLIENTS, -1);
        for (int i = 0; i<events_num; i++) {
            sock = events[i].data.fd;
            addrlen = sizeof addr;
            recvfrom(sock, &msg, sizeof msg, 0, (sa) &addr, &addrlen);
            if (msg.type == connected){
                add_client(&addr, addrlen, sock, msg.content.name);
            }
            else {
                int j = 0;
                while (j<MAX_CLIENTS){
                    if (memcmp(&clients[j].addr, &addr, addrlen)==0)
                        break;
                    j++;
                }
                if (j==MAX_CLIENTS)
                    continue;
    
                get_message(&clients[j], &msg);
            }
        }
    }
    return 0;
}