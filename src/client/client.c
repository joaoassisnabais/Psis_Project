#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <unistd.h>
#include <ncurses.h>    
#include <string.h>

#include "client.h"
#include "../common/udp.h"
#include "../common/message.h"
#include "../common/chase.h"


int main(int argc, char **argv ){;
    char client_addr[108];
    struct sockaddr_un serv_addr;

    init_window(my_win, message_win);
    cbreak();				/* Line buffering disabled	*/

    sprintf(client_addr, "/tmp/client%d", getpid());
    dgram_socket = unix_socket_init(client_addr);
    serv_addr = getAddr(argv[1]);

    client_loop(serv_addr);
    
    unlink(client_addr);
    exit(0);
}

//create client_loop that opens a udp socket and sends a connect message to the server and then implements a while that allows the user to move the player and sends the new position to the server unless the user presses q to quit or esc
void client_loop(struct sockaddr_un serv_addr){
    char msg[100];

    player_position_t *my_player;
    my_player = init_client(my_win);
    
    sprintf(msg, "connect");
    send_msg(msg, serv_addr);
    receive(my_player);

    int key=-1;
    while(key != 'q' && key != 27){
        key = wgetch(my_win);

        if (key == KEY_LEFT || key == KEY_RIGHT || key == KEY_UP || key == KEY_DOWN){
            wprintw(my_win, "key: %d\n", key);
            sprintf(msg, "move %d", key);
            send_msg(msg, serv_addr);
            receive(my_player);
        }
        if(my_player->health == 0){
            break;
        }
    }
    send_msg("disconnect", serv_addr);
    free(my_player);
}

void send_msg(char *msg_txt, struct sockaddr_un servaddr){
    char command[16];
    message msg;
    strncpy(msg.txt, msg_txt, MSG_TXT_SIZE);
    sscanf(msg.txt, "%s", command);

    if(strcmp(command, "connect") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "move") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "disconnect") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    }

}

void receive(player_position_t *my_player){
    message buffer;
    char command[16];

    if(recv(dgram_socket, &buffer, sizeof(buffer), 0) == 0){
        perror("Error receiving message, message is empty");
        exit(-1);
    }

    sscanf(buffer.txt, "%s", command);
    
    if(strcmp(command, "ball_info") == 0){
        sscanf(buffer.txt, "%*s %d %d %c", &my_player->x, &my_player->y, &my_player->c);
        my_player->health = MAX_HP;
        render_message(buffer, my_win, message_win);        

    }else if(strcmp(command, "field_status") == 0){
        sscanf(buffer.txt, "%*s %d %d %d", &my_player->x, &my_player->y, &my_player->health);
        render_message(buffer, my_win, message_win);

    }else if(strcmp(command, "dead") == 0){
        my_player->health = 0;
        render_message(buffer, my_win, message_win);
    }

}