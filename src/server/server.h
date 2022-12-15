#ifndef SERVER_H
#define SERVER_H

#include "../common/defs.h"
#include "../common/message.h"
#include "../common/list.h"
#include "../common/prizes.h"

client *head_clients = NULL;
client *head_bots = NULL;
prize *head_prizes = NULL;

WINDOW *my_win = NULL;
WINDOW *message_win = NULL;

bool movePlayer(message *msg, player_position_t *p);
void serverLoop(char* addr);
void runProcesses(message *msg, char *address);


#endif