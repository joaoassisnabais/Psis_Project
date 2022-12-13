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
    prize *new = addPrize(init_prize(my_win), head_prizes);
    while (1)
    {    
        
        //Receive the message from the client
        if(recvfrom(udp_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, &clientAddrLen) == -1){
            perror("Error receiving message");
            exit(-1);
        }; 

        strcpy(ip_address, inet_ntoa(clientAddr.sin_addr));
        run_processes(&msg, ip_address);

        if(strcmp(msg.txt, "no_reply") != 0){   /* If the client is disconnected or the message isn't listed, don't send anything */
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
            
            create_message(msg, msg_txt, getPlayersArray(head_clients), getPlayersArray(head_clients), getPlayersArray(head_clients));

        } else if (strcmp(command, "move") == 0) { /* Move */
            if(getClient(ip_adress,head_clients)->p->health <= 0){ /* Healt_0 check */
                removeClient(ip_adress, head_clients);
                create_message(msg, "dead", getPlayersArray(head_clients), getPlayersArray(head_clients), getPlayersArray(head_clients));
                return;
            }
            
            int move_key;
            sscanf(msg->txt, "%*s %d", move_key);   /* Get the movement key */
            player_position_t *current = getClient(ip_adress, head_clients)->p;
            updatePosition(current, move_key);

            char msg_txt[20];
            sprintf(msg_txt, "field_status %d %d %d", current->x, current->y, current->health);

            create_message(msg, msg_txt, getPlayersArray(head_clients), getPlayersArray(head_clients), getPlayersArray(head_clients));

        } else if (strcmp(command, "disconnect") == 0) { /* Disconnect */
            removeClient(ip_adress, head_clients);
            strcpy(msg->txt, "no_reply");

        } else if (strcmp(command, "bot_connect") == 0) { /* bot connect */
            client *new = addClient(ip_adress, init_client(my_win), head_clients);
             

        } else { /* Not a valid command */
            strcpy(msg->txt, "no_reply");
        }
        return;
}