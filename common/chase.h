#include <ncurses.h>

#ifndef CHASE_H
#define CHASE_H

player_position_t *init_client(WINDOW *my_win);
void updatePosition(player_position_t *player, int direction);

#endif