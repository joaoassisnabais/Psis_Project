#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "prizes.h"
#include "defs.h"

void create_message(message *msg, char *msg_txt, array_and_size *players, array_and_size *bots, array_and_size_prizes *prizes){ 
    if(msg->txt!=NULL) strncpy(msg->txt, msg_txt, MSG_TXT_SIZE);
    msg->num_players = players->size;
    msg->num_bots = bots->size;
    msg->num_prizes = prizes->size;
    
    for(int i = 0; i < players->size; i++){
        msg->players[i] = players->array[i];
    }
    for(int i = 0; i < bots->size; i++){
        msg->bots[i] = bots->array[i];
    }
    for(int i = 0; i < prizes->size; i++){
        msg->prizes[i] = prizes->array[i];
    }
    freeArrayAndSize(players);
    freeArrayAndSize(bots);
    freeArrayAndSize_prizes(prizes);
}

//free array_and_size allocated for listToArray
void freeArrayAndSize(array_and_size *array) {
    free(array->array);
    free(array);
}

void freeArrayAndSize_prizes(array_and_size_prizes *array) {
    free(array->array);
    free(array);
}