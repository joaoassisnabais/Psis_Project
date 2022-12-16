#include <ncurses.h>
#include "../common/defs.h"
#include "../common/message.h"

#ifndef CHASE_H
#define CHASE_H

void init_window(WINDOW *my_win, WINDOW *message_win);
void render_message(message msg, WINDOW *my_win, WINDOW *message_win);
void move_player (player_position_t *player, int direction);
void draw_player(WINDOW *win, player_position_t * player, bool delete);


#endif