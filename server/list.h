#include "defs.h"

#ifndef LIST_H
#define LIST_H

typedef struct client{
    char ip_adress[20]; 
    player_position_t *p;

    struct client *next;
} client;

typedef struct list_to_array{
    player_position_t *array;
    int size;
} array_and_size;

client * addClient(char *ip_adress, player_position_t *p, client *head_clients);
void removeClient(char *ip_adress, client *head_clients); 
client *getLastClient(client *head_clients);
client *getClient(char *ip_adress, client *head_clients);
client *getClientByChar(char c, client *head_clients);
player_position_t *getPlayersArray(client *head_clients);
void freeArrayAndSize(array_and_size *array);
void freeList(client *head_clients);

#endif