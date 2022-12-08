#include <ncurses.h>

#ifndef CHASE_H
#define CHASE_H

typedef struct player_position_t{
    int x, y;
    char c;
} player_position_t;

player_position_t *init_client(WINDOW *my_win);

#endif