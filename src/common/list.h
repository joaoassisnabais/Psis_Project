#include "defs.h"
#include "message.h"

#ifndef LIST_H
#define LIST_H

typedef struct client{
    char address[20]; 
    player_position_t *p;

    struct client *next;
} client;

//client *head_clients;
client * addClient(char *address, player_position_t *p, client *head_clients);
void removeClient(char *address, client *head_clients); 
client *getLastClient(client *head_clients);
client *getClient(char *address, client *head_clients);
client *getClientByChar(char c, client *head_clients);
client *getClientByPos(int x, int y, client *head_clients);
array_and_size*getPlayersArray(client *head_clients);
void freeList(client *head_clients);


#endif