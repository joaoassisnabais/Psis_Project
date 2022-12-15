#include "../common/defs.h"
#include "../common/message.h"

client *head_clients = NULL;
client *head_bots = NULL;

WINDOW *my_win = NULL;
WINDOW *message_win = NULL;

#ifndef SERVER_H
#define SERVER_H

bool movePlayer(message *msg, player_position_t *p);

#endif