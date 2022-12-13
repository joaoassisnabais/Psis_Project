#include "defs.h"
#include "prizes.h"

#ifndef LIST_H
#define LIST_H

typedef struct client{
    char ip_adress[20]; 
    player_position_t *p;

    struct client *next;
} client;

typedef struct prize{
    prize_pos *pr;

    struct prize *next;
} prize;

typedef struct list_to_array{
    player_position_t *array;
    int size;
} array_and_size;

client * addClient(char *ip_adress, player_position_t *p, client *head_clients);
void removeClient(char *ip_adress, client *head_clients); 
client *getLastClient(client *head_clients);
client *getClient(char *ip_adress, client *head_clients);
client *getClientByChar(char c, client *head_clients);
array_and_size*getPlayersArray(client *head_clients);
void freeArrayAndSize(array_and_size *array);
void freeList(client *head_clients);
prize *addPrize(prize *pr, prize *head_prizes);

#endif