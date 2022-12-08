#include "chase.h"

#define WINDOW_SIZE 20
#define MSG_LEN 128


#ifndef SERVER_H
#define SERVER_H

typedef struct client{
    char ip_adress[128]; /* type? */
    int health;
    player_position_t *p;

    struct client *next;
} client;

void addClient(char *ip_adress, int health, player_position_t p, client *head); 

#endif