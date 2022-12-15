#include <ncurses.h>
#include "defs.h"
#include "message.h"

#ifndef CHASE_H
#define CHASE_H

player_position_t *init_client(WINDOW *my_win);
void updatePosition(player_position_t *player, int direction);
void init_window();
void render_message(message msg, WINDOW *my_win, WINDOW *message_win);


#endif