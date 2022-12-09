
#ifndef LIST_H
#define LIST_H

typedef struct player_position_t{
    int x, y, health;
    char c;
} player_position_t;

typedef struct client{
    char ip_adress[128]; /* type? */
    player_position_t *p;

    struct client *next;
} client;

client * addClient(char *ip_adress, player_position_t *p, client *head_clients);
void removeClient(char *ip_adress, client *head_clients); 
client *getLastClient(client *head_clients);
client *getClient(char *ip_adress, client *head_clients);
client *getClientByChar(char c, client *head_clients)

#endif