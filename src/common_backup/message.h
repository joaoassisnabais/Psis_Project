#ifndef MESSAGE_H
#define MESSAGE_H

#include "defs.h"
#include "prizes.h"


typedef struct list_to_array{
    player_position_t *array;
    int size;
} array_and_size;

void create_message(message *msg, char *msg_txt, array_and_size *players, array_and_size *bots, array_and_size_prizes *prizes);
void freeArrayAndSize(array_and_size *array);
void freeArrayAndSize_prizes(array_and_size_prizes *array);

#endif
