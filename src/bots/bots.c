#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>

#include "bots.h"
#include "../common/defs.h"
#include "../common/message.h"

int main(int argc, char **argv ){
    char bot_addr[108];
    struct sockaddr_un serv_addr;

    sscanf(argv[2], "%d", &num_bots);

    sprintf(bot_addr, "/tmp/client%d", getpid());
    dgram_socket = unix_socket_init(bot_addr);
    serv_addr = getAddr(argv[1]);

    bot_loop(serv_addr);
    
    exit(0);
}

//create bot that opens a udp socket and sends a connect message to the server and then implements a while that allows the user to move the player and sends the new position to the server unless the user presses q to quit or esc
void bot_loop(struct sockaddr_un serv_addr){
    char bot_array[24];
    char msg[100];
    
    strcpy(msg, "bot_connect");
    send_msg(msg, serv_addr);

    while(1){
        for(int i = 0; i < num_bots; i=i+2){
            bot_array[i] = rand() % 4 + '0';
            bot_array[i+1] = ' ';
        }
        strcpy(msg, "bot_move ");
        strcat(msg, bot_array);
        send_msg(msg, serv_addr);
        sleep(3);
    }
    send_msg("bot_disconnect", serv_addr);
    close(dgram_socket);
}

void send_msg(char *msg_txt, struct sockaddr_un servaddr){
    char command[16];
    message msg;
    sscanf(msg_txt, "%s", command);
    msg.state.num_bots = num_bots;
    strncpy(msg.txt, msg_txt, MSG_TXT_SIZE);


    if(strcmp(command, "bot_connect") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "bot_move") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "bot_disconnect") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    }
}
