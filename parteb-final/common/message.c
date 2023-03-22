#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "defs.h"


void realloc_players(message *msg){
    msg->state.players = (player_position_t *) malloc(sizeof(player_position_t) * msg->state.num_players);
}

/*
    Copies game state to message
*/
void copy_state(message *msg, game *state){
    msg->state.num_bots=state->num_bots;
    msg->state.num_players=state->num_players;
    msg->state.num_prizes=state->num_prizes;
    for(int i=0; i<msg->state.num_bots; i++){
        msg->state.bots[i]=state->bots[i];
    }
    for(int i=0; i<msg->state.num_prizes; i++){
        msg->state.prizes[i]=state->prizes[i];
    }
    realloc_players(msg);
    memcpy(msg->state.players, state->players, sizeof(player_position_t) * msg->state.num_players);
}

/*
    Creates message from txt and game state
*/
void create_message(message *msg, char *msg_txt, game *state){ 
    if (msg_txt != NULL) strncpy(msg->txt, msg_txt, MSG_TXT_SIZE);
    if (state != NULL) copy_state(msg, state);
}

void alloc_players(message *msg){
    msg->state.players = (player_position_t *) malloc(sizeof(player_position_t) * msg->state.num_players);
}

void free_players(message *msg){
    free(msg->state.players);
}