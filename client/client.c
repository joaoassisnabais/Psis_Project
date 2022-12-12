#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "client.h"
#include "chase.h"
#include "defs.h"
#include "udp.h"
#include "message.h"


int main(int argc, char **argv ){
    int my_port, serv_port;
    char my_ip[20], serv_ip[20];

    init_window();
    cbreak();				/* Line buffering disabled	*/

    strcpy(&my_ip,argv[1]);
    sscanf(argv[2], "%d", &my_port);
    strcpy(&serv_ip,argv[3]);
    sscanf(argv[4], "%d", &serv_port);

    udp_socket = udp_socket_init(my_ip, my_port);

    client_loop(serv_ip, serv_port);
    
    exit(0);
}

//create client_loop that opens a udp socket and sends a connect message to the server and then implements a while that allows the user to move the player and sends the new position to the server unless the user presses q to quit or esc
void client_loop(char *ip, int port){
    char buffer[100];
    char message[100];

    player_position_t *player = init_client(my_win);

    struct sockaddr servaddr = getAddr(ip, port);
    
    sprintf(message, "connect");

    send_msg(&message, &servaddr);

    //break the loop if player is dead  


    int key=-1;
    while(key != 'q' && key != 27){
        key = wgetch(my_win);

        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
            sprintf(message, "move %d", key);
            send(&message, &servaddr);
        }
        receive(servaddr, player);
        if(player->health == 0){
            break;
        }
    }
    send("disconnect", &servaddr);
    free(player);
    close(udp_socket);
}

void send(void *msg, struct sockaddr *servaddr){
    char command[16];
    sscanf(msg, "%s", command);

    if(strcmp(command, "connect") == 0){
        sendto(udp_socket, &msg, strlen(msg), 0, &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "move") == 0){
        sendto(udp_socket, &msg, strlen(msg), 0, &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "disconnect") == 0){
        sendto(udp_socket, &msg, strlen(msg), 0, &servaddr, sizeof(servaddr));
    }

}

void receive(struct sockaddr servaddr, player_position_t *player){
    void *buffer;
    char command[16];
    int n;

    buffer = malloc(MSG_TXT_SIZE + MSG_WIN_SIZE);

    if(recvfrom(udp_socket, &buffer, sizeof(buffer), 0, &servaddr, sizeof(servaddr)) != MSG_TXT_SIZE + MSG_WIN_SIZE){
        perror("Error receiving message, message is not the correct size");
        exit(-1);
    }

    message msg = read_message(buffer);
    sscanf(msg.txt, "%s", command);

    if(strcmp(command, "ball_info") == 0){
        sscanf(msg.txt, "%*s %d %d %c", player->x, player->y, player->c);
        player->health = MAX_HP;
        *my_win = msg.win;
        wrefresh(my_win);
        //merdas para dar print na msg_win

    }else if(strcmp(command, "field_status") == 0){
        sscanf(msg.txt, "%*s %d %d %d", player->x, player->y, player->health);
        *my_win = msg.win;
        wrefresh(my_win);
        //merdas para dar print na msg_win

    }else if(strcmp(command, "dead") == 0){
        player->health = 0;
        *my_win = msg.win;
        wrefresh(my_win);
        //merdas para dar print na msg_win
    }

    free(buffer);
}