#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include "bots.h"
#include "defs.h"
#include "udp.h"
#include "message.h"


int main(int argc, char **argv ){
    int my_port, serv_port;
    char my_ip[20], serv_ip[20];

    strcpy(&my_ip,argv[1]);
    sscanf(argv[2], "%d", &my_port);
    strcpy(&serv_ip,argv[3]);
    sscanf(argv[4], "%d", &serv_port);
    sscanf(argv[5], "%d", &num_bots);

    udp_socket = udp_socket_init(my_ip, my_port);

    bot_loop(serv_ip, serv_port);
    
    exit(0);
}

//create bot that opens a udp socket and sends a connect message to the server and then implements a while that allows the user to move the player and sends the new position to the server unless the user presses q to quit or esc
void bot_loop(char *ip, int port){
    int bot_array[10];
    char message[100];

    struct sockaddr servaddr = getAddr(ip, port);
    
    sprintf(message, "bot_connect %d", num_bots);
    send_msg(&message, &bot_array, servaddr);

    while(1){
        for(int i = 0; i < num_bots; i++){
            bot_array[i] = rand() % 4;
        }
        strcpy(message, "bot_move");
        send_msg(&message, &bot_array, servaddr);
        sleep(3);
    }
    send_msg("bot_disconnect", &bot_array, servaddr);
    close(udp_socket);
}

void send_msg(char *msg_txt, int *bot_array, struct sockaddr servaddr){
    char command[16];
    sscanf(msg_txt, "%s", command);

    if(strcmp(command, "bot_connect") == 0){
        sendto(udp_socket, &msg_txt, strlen(msg_txt)+1, 0, &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "bot_move") == 0){
        sendto(udp_socket, bot_array, sizeof(int)*10, 0, &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "bot_disconnect") == 0){
        sendto(udp_socket, &msg_txt, strlen(msg_txt)+1, 0, &servaddr, sizeof(servaddr));
    }
}
