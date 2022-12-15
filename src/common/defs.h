#ifndef DEFS_H
#define DEFS_H

#include <stddef.h>

#define WINDOW_SIZE 20
#define MSG_TXT_SIZE 1024
#define MAX_HP 10


typedef struct player_position_t{
    int x, y, health;
    char c;
} player_position_t;

#endif
