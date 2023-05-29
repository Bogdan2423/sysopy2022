#include <mqueue.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "header.h"

mqd_t my_queue, main_queue, my_id;
char my_queue_name[128];
pid_t child_pid;
char my_id_string[3];
char content[MAX_SIZE];

void send_stop(int id){
    printf("Disconnecting...\n");
    sprintf(content,"%d%s000",STOP,my_id_string);
    mq_send(main_queue, content, MAX_SIZE, 0);
    mq_close(my_queue);
    mq_unlink(my_queue_name);
    kill(child_pid, SIGKILL);
    exit(0);
}

int get_my_id(char *content){
    char id[3];
    for (int i=0; i<3; i++)
        id[i]=content[i+1];
    return atoi(id);
}

int get_sender_id(char* content){
    char id[3];
    for (int i=0; i<3; i++)
        id[i]=content[i+1];
    return atoi(id);
}

int main(int argc, char* argv[]){
    srand(time(NULL));
    struct mq_attr atr;

    atr.mq_flags=0;
    atr.mq_maxmsg=10;
    atr.mq_msgsize=MAX_SIZE;
    atr.mq_curmsgs=0;
    sprintf(my_queue_name, "/queue%d", rand());

    char in_content[MAX_SIZE];
    char out_content[MAX_SIZE];

    main_queue = mq_open(QUEUE_NAME, O_RDWR);
    my_queue = mq_open(my_queue_name,O_CREAT|O_RDWR,0644,&atr);

    sprintf(out_content,"%d000000%s",INIT, my_queue_name);

    mq_send(main_queue, out_content, MAX_SIZE, 0);
    mq_receive(my_queue, in_content, MAX_SIZE, NULL);
    my_id=get_my_id(in_content);
    printf("Connected with id %d\n",my_id);
    if (my_id<10){
        my_id_string[2]=my_id+'0';
        my_id_string[1]='0';
        my_id_string[0]='0';
    }
    else if (my_id<100){
        my_id_string[2]=my_id%10+'0';
        my_id_string[1]=my_id/10+'0';
        my_id_string[0]='0';
    }
    else{
        my_id_string[2]=((my_id%100)%10)+'0';
        my_id_string[1]=((my_id/10)%10)+'0';
        my_id_string[0]=my_id/100+'0';
    }
    time_t t;
    struct tm tm;
    char timestr[128];
    char out_id_string[3];
    int out_id;
    char input[MAX_SIZE-132];

    child_pid=fork();
    if (child_pid==0){
        while(1){
            fgets(input,MAX_SIZE,stdin);
            if (strcmp(input,"STOP\n")==0){
                kill(getppid(),SIGINT);
            }
            if (strcmp(input,"LIST\n")==0){
                sprintf(out_content,"%d%s000",LIST,my_id_string);
                mq_send(main_queue, out_content, MAX_SIZE, 0);
            }
            if (strcmp(input,"2ALL\n")==0){
                sprintf(out_content,"%d%s000",ALL,my_id_string);
                printf("Message: ");
                fgets(input, MAX_SIZE, stdin);
                t = time(NULL);
                tm = *localtime(&t);
                sprintf(timestr, "%d-%02d-%02d %02d:%02d:%02d: ",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                strcat(out_content,timestr);
                strcat(out_content,input);
                mq_send(main_queue, out_content, MAX_SIZE, 0);
            }
            if (strcmp(input,"2ONE\n")==0){
                printf("Client id: ");
                fgets(input, MAX_SIZE-132, stdin);
                out_id=atoi(input);
                if (out_id<10){
                    out_id_string[2]=out_id+'0';
                    out_id_string[1]='0';
                    out_id_string[0]='0';
                }
                else if (out_id<100){
                    out_id_string[2]=out_id%10+'0';
                    out_id_string[1]=out_id/10+'0';
                    out_id_string[0]='0';
                }
                else{
                    out_id_string[2]=((out_id%100)%10)+'0';
                    out_id_string[1]=((out_id/10)%10)+'0';
                    out_id_string[0]=out_id/100+'0';
                }
                sprintf(out_content,"%d%s%s",ONE,my_id_string,out_id_string);
                
                printf("Message: ");
                fgets(input, MAX_SIZE-132, stdin);
                t = time(NULL);
                tm = *localtime(&t);
                sprintf(timestr, "%d-%02d-%02d %02d:%02d:%02d: ",
                tm.tm_year+1900, tm.tm_mon+1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
                strcat(out_content,timestr);
                strcat(out_content,input);
                mq_send(main_queue, out_content, MAX_SIZE, 0);
            }
        }
        return 0;
    }
    else{
        signal(SIGINT, send_stop);
        while(1){
            mq_receive(my_queue, in_content, MAX_SIZE,NULL);
            if(in_content[0]-'0'==STOP)
                raise(SIGINT);
            else if(in_content[0]-'0'==LIST)
                printf("%s\n", in_content+1);
            else if (in_content[0]-'0'==ALL){
                printf("Client %d on %s\n",
                get_sender_id(in_content),
                in_content+7);
            }
            else if (in_content[0]-'0'==ONE){
                printf("Client %d on %s\n",
                get_sender_id(in_content),
                in_content+9);
            }
        }
    }
    return 0;
}

//ipcs -q