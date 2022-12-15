#include <ncurses.h>
#include "../common/defs.h"
#include "../common/message.h"

#ifndef CHASE_H
#define CHASE_H

player_position_t *init_client(WINDOW *my_win);
void init_window(WINDOW *my_win, WINDOW *message_win);
void render_message(message msg, WINDOW *my_win, WINDOW *message_win);
void new_player_position (player_position_t *player, WINDOW *my_win);
void new_bot_position (player_position_t *bot, WINDOW *my_win);
void move_player (player_position_t *player, int direction);
void draw_player(WINDOW *win, player_position_t * player, bool delete);


#endif