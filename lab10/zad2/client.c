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
#include <time.h>
#include "shared.h"

int sock;
char enemy[NAME_LEN];
struct game_state game;
char my_symbol;

void connect_via_web(char* ipv4, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, ipv4, &addr.sin_addr) <= 0) {
        printf("Wrong address\n");
        exit(0);
    }
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    connect(sock, (struct sockaddr*) &addr, sizeof addr);
}


void connect_via_unix(char* path, char* user) {
    struct sockaddr_un addr, bind_addr;
    memset(&addr, 0, sizeof(addr));
    bind_addr.sun_family = AF_UNIX;
    addr.sun_family = AF_UNIX;
    snprintf(bind_addr.sun_path, sizeof bind_addr.sun_path, "/tmp/%s%ld", user, time(NULL));
    strncpy(addr.sun_path, path, sizeof addr.sun_path);

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);	
    bind(sock, (void*) &bind_addr, sizeof addr);
    connect(sock, (struct sockaddr*) &addr, sizeof addr);
}

void print_board(){
    for (int i = 0; i<9; i++){
        printf("%c ", game.board[i]);
        if ((i+1)%3==0)
            printf("\n");
    }
}

void handler(int nothing) {
    message msg = {.type = disconnected};
    send(sock, &msg, sizeof msg, 0);
    exit(0);
}


int main(int argc, char * argv[]){
    char name[NAME_LEN];
    if (argc == 5 && strcmp(argv[2], "web") == 0){
        strcpy(name, argv[1]);
        connect_via_web(argv[3], atoi(argv[4]));
    }
    else if (argc == 4 && strcmp(argv[2], "unix") == 0){
        strcpy(name, argv[1]);
        connect_via_unix(argv[3], name);
    }
    else {
        printf("Usage: client [name] [web/unix] [address]\n");
        exit(0);
    }

    signal(SIGINT, handler);

    for (int i = 0; i<9; i++)
        game.board[i]='-';

    message msg = { .type = connected };
    strncpy(msg.content.name, name, NAME_LEN);
    send(sock, &msg, sizeof msg, 0);

    int epoll_fd = epoll_create1(0);
    
    struct epoll_event stdin_event = { 
        .events = EPOLLIN | EPOLLPRI,
        .data = { .fd = STDIN_FILENO }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FILENO, &stdin_event);

    struct epoll_event socket_event = { 
        .events = EPOLLIN | EPOLLPRI | EPOLLHUP,
        .data = { .fd = sock }
    };
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &socket_event);

    int input;
    struct epoll_event events[2];
    while(1) {
        int n = epoll_wait(epoll_fd, events, 2, 1);
        for (int i = 0; i<n; i++){
            if (events[i].data.fd == STDIN_FILENO) {
                scanf("%d", &input);
                message msg = { .type = moved };
                msg.content.move = input;
                sendto(sock, &msg, sizeof msg, 0, NULL, sizeof(struct sockaddr_in));
            }
            else {
                recvfrom(sock, &msg, sizeof msg, 0, NULL, NULL);
                if (msg.type == start) {
                    strncpy(enemy, msg.content.name, NAME_LEN);
                    my_symbol = msg.content.symbol;
                    printf("You are playing against %s\nYour symbol: %c\n", enemy, my_symbol);
                    for (int j = 0; j<9; j++){
                        printf("%d",j);
                        if ((j+1)%3==0)
                            printf("\n");
                    }
                }
                if (msg.type == state) {
                    memcpy(&game, &msg.content.state, sizeof game);
                    print_board();
                }
                if (msg.type == end) {
                    if (msg.content.symbol == my_symbol)
                        printf("You win\n");
                    else if (msg.content.symbol == '-') 
                        printf("Draw\n");
                    else 
                        printf("You lose\n");
                    exit(0);
                }
                if (msg.type == wait) {
                    printf("Waiting for an opponent\n");
                } 
                if (msg.type == server_full) {
                    printf("Server is full\n");
                    close(sock);
                    exit(0);
                } 
                if (msg.type == username_taken) {
                    printf("This username is already taken\n");
                    close(sock);
                    exit(0);
                }
                if (msg.type == disconnected) {
                    printf("Disconnected\n");
                    exit(0);
                } 
                if (msg.type == ping){
                    write(sock, &msg, sizeof msg);
                }
            }
        }
    }

    return 0;
}
