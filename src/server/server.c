#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "server.h"
#include "defs.h"
#include "message.h"
#include "list.h"
#include "chase.h"
#include "prizes.h"


int main(int argc, char **argv){
    int port;
    char ip[16];

    init_window();

    strcpy(ip,argv[1]);
    sscanf(argv[2], "%d", &port);
    server_loop(ip, port);
    
    exit(0);
}


void server_loop(char* addr, char* port){
    char ip_address[20];
    message msg;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen;

    int udp_socket = udp_socket_init(addr, port);

    for (int i = 0; i < 5; i++) {
        prize *new = addPrize(init_prize(my_win), head_prizes);
    }

    time_t time0 = clock();
    while (1)
    {
        time0 = updatePrizes(my_win, time0);
        //Receive the message from the client
        if(recvfrom(udp_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, &clientAddrLen) == -1){
            perror("Error receiving message");
            exit(-1);
        }; 

        strcpy(ip_address, inet_ntoa(clientAddr.sin_addr));
        run_processes(&msg, ip_address);

        if(strcmp(msg.txt, "no_reply") != 0){   /* If the client is disconnected, the message isn't listed or the message came from a bot, don't send anything */
            if (sendto(udp_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, &clientAddrLen) != sizeof(msg) ){
                perror("Full message wasn't sent");
                exit(-1);
            }
        }       
    }
}

void run_processes(message *msg, char *ip_adress){
        char command[16];
        sscanf(msg->txt, "%s", command);

        if (strcmp(command, "connect") == 0) {  /* Connect */
            client *new = addClient(ip_adress, init_client(my_win), head_clients);
            char msg_txt[20];
            sprintf(msg_txt, "ball_info %d %d %c", new->p->x, new->p->y, new->p->c);
            create_message(msg, msg_txt, getPlayersArray(head_clients), getPlayersArray(head_bots), getPrizesArray(head_prizes));

        } else if (strcmp(command, "move") == 0) { /* Move */
            if(movePlayer(msg, getClient(ip_adress, head_clients)->p) == false){
                removeClient(ip_adress, head_clients);
                create_message(msg, "dead", getPlayersArray(head_clients), getPlayersArray(head_bots), getPrizesArray(head_prizes));
                return;
            }
            create_message(msg, NULL, getPlayersArray(head_clients), getPlayersArray(head_bots), getPrizesArray(head_prizes));

        } else if (strcmp(command, "disconnect") == 0) { /* Disconnect */
            removeClient(ip_adress, head_clients);
            strcpy(msg->txt, "no_reply");

        } else if (strcmp(command, "bot_connect") == 0) { /* bot connect */
            initBots(msg->num_bots, ip_adress, head_bots);
            strcpy(msg->txt, "no_reply");

        }else if (strcmp(command, "bot_move") == 0){    /* bot move */
            moveBots(*msg);
            strcpy(msg->txt, "no_reply");
        
        } else { /* Not a valid command */
            strcpy(msg->txt, "no_reply");
        }
        return;
}

void moveBots(message msg){
    int direction[10], i=-1;
    char *token;

    token = strtok(msg.txt, " ");
    token = strtok(NULL, " ");
    while( token != NULL ){
        i++;
        direction[i] = atoi(token);
        token = strtok(NULL, " ");
    }
    for (int i = 0; i < 10; i++){ 
        if (direction[i] == 0){
            direction[i] = KEY_UP;
        }else if (direction[i] == 1){
            direction[i] = KEY_RIGHT;
        }else if (direction[i] == 2){
            direction[i] = KEY_DOWN;
        }else if (direction[i] == 3){
            direction[i] = KEY_LEFT;
        }
    }

    client *current = head_bots;
    while (current != NULL){
        updatePosition(current->p, direction);
        current = current->next;
    }
}

bool movePlayer(message *msg, player_position_t *p){

    if(p->health <= 0){ /* Healt_0 check */
        return false;
    }
                
    int move_key;
    sscanf(msg->txt, "%*s %d", move_key);   /* Get the movement key */
    updatePosition(p, move_key);

    char msg_txt[20];
    sprintf(msg_txt, "field_status %d %d %d", p->x, p->y, p->health);
    strcpy(msg->txt, msg_txt);

    return true;
}
