#ifndef CLIENT_H
#define CLIENT_H

#include <ncurses.h>
#include "../common/defs.h"

int dgram_socket;
WINDOW *my_win = NULL;
WINDOW *message_win = NULL;

void receive(player_position_t *my_player);
void client_loop(struct sockaddr_un serv_addr);

#endif