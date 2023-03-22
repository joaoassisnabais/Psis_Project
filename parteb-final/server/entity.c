#define PLAYER_TIMEOUT 10

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

static int player_fd[WINDOW_SIZE*WINDOW_SIZE];
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
    aux.c = 'A' + letter_buffer;
    letter_buffer++;
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

void addPlayer(game *state, int cfd){
    state->players = realloc(state->players, sizeof(player_position_t)*(state->num_players+1));
    state->players[state->num_players]=new_player_position(state);
    player_fd[state->num_players]=cfd;
    state->num_players+=1;
}

void rmPlayer(game *state, player_position_t *player){
    for(int i=0; i<state->num_players; i++){
        if(state->players[i].c == player->c){
            for(int j=i; j<state->num_players-1; j++){
                state->players[j] = state->players[j+1];
                player_fd[j] = player_fd[j+1];
            }
            state->num_players-=1;
            state->players=realloc(state->players, sizeof(player_position_t)*(state->num_players));
            return;
        }
    }
}

void rmPlayerByAddr(game *state, int cfd){
    for(int i=0; i<state->num_players; i++){
        if(player_fd[i] == cfd){
            for(int j=i; j<state->num_players; j++){
                state->players[j] = state->players[j+1];
                player_fd[j] = player_fd[j+1];
            }
            state->num_players-=1;
            state->players=realloc(state->players, sizeof(player_position_t)*(state->num_players));
            return;
        }
    }
}

void initBots(int num_bots, game *state) {
    for (int i = 0; i < num_bots; i++) {
        addBot(state);
    }
}


void addBot(game *state){
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

bool timeout_dead(time_t time0){
    if (time(NULL)-time0 >= PLAYER_TIMEOUT){
        return true;
    }
    return false;
}

player_position_t *getPlayerByAddr(int fd, game *state){
    for(int i=0; i<state->num_players; i++){
        if(player_fd[i] == fd) return &state->players[i];
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

bool PlayerAuth(int fd, int max_players){
    for(int i=0; i<max_players; i++){
        if(player_fd[i] == fd) return true;
    }
    return false;
}

int getPlayerFd(game *state, player_position_t *player){
    for(int i=0; i<state->num_players; i++){
        if(state->players[i].c == player->c){
            return player_fd[i];
        }
    }
    return -1;
}