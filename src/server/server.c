#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "server.h"
#include "../common/list.h"
#include "../common/udp.h"
#include "../common/chase.h"
#include "../common/prizes.h"

#define SOCK_PATH "/tmp/server_socket.sock"

void updatePosition(player_position_t *player, int direction){
    player_position_t aux = *player;
    bool is_bot, is_empty=true;
    move_player(&aux, direction);

    if(player->c == '*') is_bot = true;
    else is_bot = false;

    if(isPlayerCol(aux.x, aux.y) || isPrizeCol(aux.x, aux.y) || isBotCol(aux.x, aux.y)){    /*collision with another player*/
        is_empty = false;
    }

    if (is_empty){    /*moves into empty space or the wall*/
        draw_player(my_win, player, true);
        player->x = aux.x;
        player->y = aux.y;
        draw_player(my_win, player, false);

    }else if((!is_bot) && (isPrizeCol(aux.x, aux.y))){         /*moves into a health pack*/
        draw_player(my_win, player, true);
        player->x = aux.x;
        player->y = aux.y;
        player->health+=getPrizeByPos(aux.x, aux.y, head_prizes)->hp;

        prize_pos *to_delete = getPrizeByPos(aux.x, aux.y, head_prizes);    /*delete the prize from screen and list*/
        draw_prize(my_win, to_delete, true);
        removePrize(to_delete, head_prizes);
        
        draw_player(my_win, player, false);

    }else if(isPlayerCol(aux.x, aux.y)){     /*moves into another player*/
        if(!is_bot) player->health++;
        getClientByPos(aux.x, aux.y, head_clients)->p->health--;
    }

    if(!is_bot && (player->health>MAX_HP)){
        player->health=MAX_HP;
    }
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
    i=0;
    while (current != NULL){
        updatePosition(current->p, direction[i]);
        current = current->next;
        i++;
    }
}


bool movePlayer(message *msg, player_position_t *p){

    if(p->health <= 0){ /* Healt_0 check */
        return false;
    }
                
    int move_key;
    sscanf(msg->txt, "%*s %d", &move_key);   /* Get the movement key */
    updatePosition(p, move_key);

    char msg_txt[20];
    sprintf(msg_txt, "field_status %d %d %d", p->x, p->y, p->health);
    strcpy(msg->txt, msg_txt);

    return true;
}

void runProcesses(message *msg, char *address){
        char command[16];
        sscanf(msg->txt, "%s", command);

        if (strcmp(command, "connect") == 0) {  /* Connect */
            client *new = addClient(address, init_client(my_win), head_clients);
            char msg_txt[20];
            sprintf(msg_txt, "ball_info %d %d %c", new->p->x, new->p->y, new->p->c);
            create_message(msg, msg_txt, getPlayersArray(head_clients), getPlayersArray(head_bots), getPrizesArray(head_prizes));

        } else if (strcmp(command, "move") == 0) { /* Move */
            if(movePlayer(msg, getClient(address, head_clients)->p) == false){
                removeClient(address, head_clients);
                create_message(msg, "dead", getPlayersArray(head_clients), getPlayersArray(head_bots), getPrizesArray(head_prizes));
                return;
            }
            create_message(msg, NULL, getPlayersArray(head_clients), getPlayersArray(head_bots), getPrizesArray(head_prizes));

        } else if (strcmp(command, "disconnect") == 0) { /* Disconnect */
            removeClient(address, head_clients);
            strcpy(msg->txt, "no_reply");

        } else if (strcmp(command, "bot_connect") == 0) { /* bot connect */
            initBots(msg->num_bots, address, head_bots, my_win);
            strcpy(msg->txt, "no_reply");

        }else if (strcmp(command, "bot_move") == 0){    /* bot move */
            moveBots(*msg);
            strcpy(msg->txt, "no_reply");
        
        } else { /* Not a valid command */
            strcpy(msg->txt, "no_reply");
        }
        return;
}

void serverLoop(char* addr){
    char address[108];
    message msg;
    struct sockaddr_un clientAddr;
    socklen_t clientAddrLen;
    clientAddrLen = sizeof(clientAddr);

    int dgram_socket = unix_socket_init(address);

    for (int i = 0; i < 5; i++) {
        addPrize(init_prize(my_win), head_prizes);
    }

    time_t time0 = clock();
    while (1)
    {
        time0 = updatePrizes(my_win, time0, head_prizes);
        //Receive the message from the client
        if(recvfrom(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, &clientAddrLen) == -1){
            perror("Error receiving message");
            exit(-1);
        }; 

        strcpy(address, clientAddr.sun_path);
        runProcesses(&msg, address);

        if(strcmp(msg.txt, "no_reply") != 0){   /* If the client is disconnected, the message isn't listed or the message came from a bot, don't send anything */
            if (sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, clientAddrLen) != sizeof(msg) ){
                perror("Full message wasn't sent");
                exit(-1);
            }
        }       
    }
    unlink(address);
}

int main(int argc, char **argv){
    char address[108];

    init_window(my_win, message_win);

    //strcpy(address,argv[1]);
    strcpy(address,SOCK_PATH);
    serverLoop(address);
    
    exit(0);
}
