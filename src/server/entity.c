#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "../common/defs.h"
#include "../common/chase_internal.h"
#include "entity.h"

static char bot_address[108];
static char player_address[WINDOW_SIZE*WINDOW_SIZE][108];
int letter_buffer=0;

player_position_t *getClientByPos(int x, int y, game *state){
    for(int i=0; i<state->num_players; i++){
        if(state->players[i].x == x && state->players[i].y == y) return &state->players[i];
    }
    return NULL;
}   

bool isPlayerCol(int x, int y, game *state){
    player_position_t *aux = getClientByPos(x, y, state);
    if(aux != NULL){
        if(aux->x == x && aux->y == y) return true;
    }
    return false;
}

bool isBotCol(int x, int y, game *state){
    for(int i=0; i<state->num_bots; i++){
        if(state->bots[i].x == x && state->bots[i].y == y) return true;
    }
    return false;
}

bool isPrizeCol(int x, int y, game *state){
    for(int i=0; i<state->num_prizes; i++){
        if(state->prizes[i].x == x && state->prizes[i].y == y) return true;
    }
    return false;
}


bool isEmpty(int x, int y, game *state){
    if(isPlayerCol(x, y, state) || isBotCol(x, y, state) || isPrizeCol(x, y, state)){
        return false;
    }
    return true;
}

player_position_t new_player_position (game *state){
    player_position_t aux;
    aux.c = 'A' + state->num_players;
    aux.health = MAX_HP;
    bool is_empty = false;
    while (!is_empty){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        is_empty = isEmpty(aux.x, aux.y, state);
    }
    return aux;
}

player_position_t new_bot_position (game *state){
    player_position_t aux;
    aux.c = '*';
    aux.health = 99999;
    bool is_empty = false;
    while (!is_empty){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        is_empty = isEmpty(aux.x, aux.y, state);
    }
    return aux;
}

void addPlayer(game *state, char* address){
    state->players = realloc(state->players, sizeof(player_position_t)*(state->num_players+1));
    state->players[state->num_players]=new_player_position(state);
    strcpy(player_address[state->num_players], address);
    state->num_players+=1;
}

void rmPlayer(game *state, player_position_t *player){
    for(int i=0; i<state->num_players; i++){
        if(state->players[i].c == player->c){
            for(int j=i; j<state->num_players-1; j++){
                state->players[j] = state->players[j+1];
                strcpy(player_address[j], player_address[j+1]);
            }
            state->num_players-=1;
            realloc(state->players, sizeof(player_position_t)*(state->num_players-1));
            return;
        }
    }
}

void rmPlayerByAddr(game *state, char *address){
    for(int i=0; i<state->num_players; i++){
        if(strcmp(player_address[i], address) == 0){
            for(int j=i; j<state->num_players; j++){
                state->players[j] = state->players[j+1];
                strcpy(player_address[j], player_address[j+1]);
            }
            state->num_players-=1;
            realloc(state->players, sizeof(player_position_t)*(state->num_players-1));
            return;
        }
    }
}

void initBots(int num_bots, char *address, game *state) {
    strcpy(bot_address, address);
    for (int i = 0; i < num_bots; i++) {
        addBot(address, state);
    }
}


void addBot(char *address, game *state){
    state->bots[state->num_bots] = new_bot_position(state);
    state->num_bots+=1;
    return;
}

prize_pos new_prize(game *state){
    prize_pos aux;
    aux.hp = 1 + (rand() % 5);
    bool is_empty = false;
    while (!is_empty){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generatesfalsea random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        is_empty = isEmpty(aux.x, aux.y, state);
    }
    return aux;
}

void addPrize(game *state){
    state->prizes[state->num_prizes] = new_prize(state);
    state->num_prizes+=1;
}

void initPrizes(game *state, int num_prizes){
    for(int i=0; i<num_prizes; i++){
        addPrize(state);
    }
}

time_t updatePrizes(time_t time0, game *state){
    if (time(NULL)-time0 >= 5 && state->num_prizes<10){
        addPrize(state);
        return time(NULL);
    }
    return time0;
}

player_position_t *getPlayerByAddr(char *address, game *state){
    for(int i=0; i<state->num_players; i++){
        if(strcmp(player_address[i], address) == 0) return &state->players[i];
    }
    return NULL;
}

prize_pos getPrizeByPos(int x, int y, game *state){
    for(int i=0; i<state->num_prizes; i++){
        if(state->prizes[i].x == x && state->prizes[i].y == y) return state->prizes[i];
    }
    prize_pos aux;
    aux.hp = 0;
    return aux;
}

void rmPrizeByPos(int x, int y, game *state){
    for(int i=0; i<state->num_prizes; i++){
        if(state->prizes[i].x == x && state->prizes[i].y == y){
            for(int j=i; j<state->num_prizes-1; j++){
                state->prizes[j] = state->prizes[j+1];
            }
            state->num_prizes-=1;
            return;
        }
    }
}

bool BotIsOn(){
    if (bot_address[0] == '\0') return false;
    else return true;
}

bool BotAuth(char *address){
    if(strcmp(address, bot_address) == 0) return true;
    return false;
}

bool PlayerAuth(char *address, game *state){
    for(int i=0; i<state->num_players; i++){
        if(strcmp(address, player_address[i]) == 0) return true;
    }
    return false;
}

void initBotAddr(){
    bot_address[0] = '\0';
}
