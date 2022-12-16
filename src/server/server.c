#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>

#include "entity.h"
#include "../common/chase_internal.h"
#include "../common/chase_frontend.h"
#include "../common/defs.h"
#include "../common/message.h"
#include "../common/connection.h"

#define SOCK_PATH "/tmp/server_socket.sock"
#define INIT_PRIZES 5
#define MAX_PLAYERS 10
#define MAX_BOTS 10


game state;
screen game_screen;
int dgram_socket;
bool no_reply=false;

void move_player (player_position_t * player, dir direction){    
    if (direction == DIR_UP){        
        if (player->y != 1){            
            player->y--;        
        }    
    }    
    if (direction == DIR_DOWN){        
        if (player->y  != WINDOW_SIZE-2){
            player->y ++;        
        }    
    }        
    if (direction == DIR_LEFT){        
        if (player->x  != 1){            
            player->x --;        
        }    
    }    
    if (direction == DIR_RIGHT) {
        if (player->x  != WINDOW_SIZE-2){            
            player->x ++;    
        } 
    }
}

/*
    Updates positions by checking for collisions and works out collisions
*/
void updatePosition(player_position_t *player, dir direction){
    player_position_t aux = *player;
    bool is_bot, is_empty;

    move_player(&aux, direction);
        is_empty=isEmpty(aux.x, aux.y, &state);
    
    if(isPlayerCol(aux.x, aux.y, &state)){
        if(getClientByPos(aux.x, aux.y, &state)->health<=0){
            is_empty=true;
        }
    }
    if(player->c == '*') is_bot = true;
    else is_bot = false;

    if (is_empty){    /*moves into empty space or the wall*/
        player->x = aux.x;
        player->y = aux.y;

    }else if((!is_bot) && (isPrizeCol(aux.x, aux.y, &state))){         /*moves into a health pack*/
        player->x = aux.x;
        player->y = aux.y;
        player->health += getPrizeByPos(aux.x, aux.y, &state).hp;
        rmPrizeByPos(aux.x, aux.y, &state);


    }else if(isPlayerCol(aux.x, aux.y, &state) && getClientByPos(aux.x, aux.y, &state)->health>0){     /*moves into another player*/
        if(!is_bot) player->health++;
        getClientByPos(aux.x, aux.y, &state)->health-=1;
    }

    if(!is_bot && (player->health>MAX_HP)){
        player->health=MAX_HP;
    }
}

void moveBots(dir *directions){
    for(int i=0; i<state.num_bots; i++){
        updatePosition(&state.bots[i], directions[i]);
    }
}

/*
    Parses incoming bot directions message
*/
void parseBotDirections(message *msg) {
    char *token;
    int i=-1;
    dir directions[10];

    token = strtok(msg->txt, " ");
    token = strtok(NULL, " ");
    while( token != NULL ){
        i++;
        directions[i] = atoi(token);
        token = strtok(NULL, " ");
    }

    moveBots(directions);
}

/*
    Checks if player is alive and updates position
*/
bool updatePlayer(player_position_t *p, dir direction){
    if(p->health <= 0){ /* Healt_0 check */
        return false;
    }else{
        updatePosition(p, direction);
        return true;
    }
}

void write_ball_info(char *buffer) {
    sprintf(buffer, "ball_info %d %d %c", state.players[state.num_players-1].x, state.players[state.num_players-1].y, state.players[state.num_players-1].c);
}

/*
    Reads incoming messge and updates game variables acordingly
*/
void parseMessage(message *msg, char *address){
        char command[16];
        char buffer[100];
        sscanf(msg->txt, "%s", command);

        if (strcmp(command, "connect") == 0) {  /* Connect */
            if(state.num_players >= MAX_PLAYERS){
                create_message(msg, "full", &state);
            }
            else{
                addPlayer(&state, address);
                write_ball_info(buffer);
                create_message(msg, buffer, &state);
            }

        } else if (strcmp(command, "move") == 0) { /* Move */
            if(PlayerAuth(address, &state) == false){
                no_reply = true;
                return;
            }
            dir direction;
            sscanf(msg->txt, "%*s %d", (int *) &direction);   /* Get the movement key */
            player_position_t *pos = getPlayerByAddr(address, &state);
            if (updatePlayer(pos, direction)) {
                create_message(msg, "field_status", &state);

            } else {
                rmPlayer(&state, pos);
                create_message(msg, "dead", &state);
            }

        } else if (strcmp(command, "disconnect") == 0) { /* Disconnect */
            if(PlayerAuth(address, &state) == false){
                no_reply = true;
                return;
            }
            rmPlayerByAddr(&state, address);
            no_reply = true;

        } else if (strcmp(command, "bot_connect") == 0) { /* bot connect */
            if(BotIsOn() || msg->state.num_bots > MAX_BOTS){
                no_reply=true;
                return;
            }
            initBots(msg->state.num_bots, address, &state);
            no_reply = true;

        }else if (strcmp(command, "bot_move") == 0){    /* bot move */
            if(BotAuth(address) == false){
                no_reply = true;
                return;
            }
            parseBotDirections(msg);
            no_reply = true;
        
        } else { /* Not a valid command */
            no_reply = true;
        }
        return;
}

/*
    Infinite server loop that receives and sends messages
*/
void serverLoop(){
    char address[108];
    message msg;
    struct sockaddr_un clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    initPrizes(&state, INIT_PRIZES);
    time_t time0 = time(NULL);
    while (1)
    {
        clientAddrLen = sizeof(clientAddr);
        time0 = updatePrizes(time0, &state);    /*checking if a new prize can be spawned*/
        
        if(recvfrom(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, &clientAddrLen) == -1){    /* Receive message */
            perror("Error receiving message");
            exit(-1);
        }; 

        parseMessage(&msg, clientAddr.sun_path);    /* Update game variables */

        //checjkar o que esta a dar de errado no remove

        if(!no_reply){      /* If the client is disconnected, the message isn't listed or the message came from a bot, don't send anything */
            if (sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, clientAddrLen) == -1){
                perror("Full message wasn't sent");
                exit(-1);
            }
        }else{
            no_reply=false;
        }
        render(game_screen, &state);
    }
    unlink(address);
}

void initGame(){
    state.num_bots=0;
    state.num_players=0;
    state.num_prizes=0;
}

int main(int argc, char **argv){
    char address[108];

    init_window(&game_screen);
    initGame();
    initBotAddr();

    if(argc==2) strcpy(address,argv[1]);
    else strcpy(address,SOCK_PATH);
    dgram_socket = unix_socket_init(address);
    serverLoop();

    exit(0);
}
