#include <ncurses.h>
#include "defs.h"

#ifndef MESSAGE_H
#define MESSAGE_H

typedef struct message{
    char txt[MSG_TXT_SIZE];
    int num_players, num_bots, num_prizes;
    player_position_t players[10];
    player_position_t bots[10];
    player_position_t prizes[10]
} message;

typedef struct list_to_array{
    player_position_t *array;
    int size;
} array_and_size;

void create_message(message *msg, char *msg_txt, array_and_size *players, array_and_size *bots, array_and_size *prizes);



void freeArrayAndSize(array_and_size *array);

#endif