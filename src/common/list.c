#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "list.h"
#include "../common/chase.h"
#include "prizes.h"

/*********************************CLIENTS**********************************/

client * addClient(char *address, player_position_t *p, client *head_clients) {
    client *newClient = (client*) malloc(sizeof(client)); 
    if(newClient == NULL) {
        perror("Error allocating memory for new client");
        exit(-1);
    }
    strcpy(newClient-> address, address);
    newClient->p = p;
    newClient->next = NULL;

    if (head_clients == NULL) {
        head_clients = newClient;
        newClient->p->c = 'A';
    } else {
        client *current = head_clients;
        while (current->next != NULL) {
            current = current->next;
        }
        newClient->p->c = current->p->c + 1;
        current->next = newClient;
    }

    return newClient;
}

//remove client from list
void removeClient(char *address, client *head_clients) {
    client *current = head_clients;
    client *previous = NULL;

    if (current == NULL) {  //no list
        return;
    }

    //runs until it finds the client or the end of the list
    while (current != NULL && strcmp(current->address, address) != 0) { 
        previous = current;
        current = current->next;
    }

    //client not found
    if (current == NULL) {
        return;
    }

    //client is first in list (1) or in the middle/end of list (2)
    if (previous == NULL) {
        head_clients = current->next;
    } else {
        previous->next = current->next;
    }

    free(current->p);
    free(current);
}

client *getLastClient(client *head_clients) {
    client *current = head_clients;

    if (current == NULL) {
        return NULL;
    }

    while (current->next != NULL) {
        current = current->next;
    }

    return current;
}

client *getClient(char *address, client *head_clients) {
    client *current = head_clients;

    if (current == NULL) {
        return NULL;
    }

    while (current != NULL && strcmp(current->address, address) != 0) {
        current = current->next;
    }

    return current;
}

client *getClientByChar(char c, client *head_clients) {
    client *current = head_clients;

    if (current == NULL) {
        return NULL;
    }

    while (current != NULL && current->p->c != c) {
        current = current->next;
    }

    return current;
}

array_and_size *getPlayersArray(client *head_clients) {
    player_position_t *array = (player_position_t*) malloc(sizeof(player_position_t) * 10);
    if (array == NULL) {
        perror("Error allocating memory for player_position_t array");
        exit(-1);
    }

    client *current = head_clients;
    int i = 0;

    while (current != NULL) {
        array[i] = *current->p;
        current = current->next;
        i++;
    }
    array_and_size *out = (array_and_size*) malloc(sizeof(array_and_size));
    out->array = array;
    out->size = i;
    
    return out;
}

//free all memory allocated for the list and delete all clients
void freeList(client *head_clients) {
    client *current = head_clients;
    client *next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current->p);
        free(current);
        current = next;
    }
}

//get client by position
client *getClientByPos(int x, int y, client *head_clients) {
    client *current = head_clients;

    if (current == NULL) {
        return NULL;
    }

    while (current != NULL && (current->p->x != x && current->p->y != y)) {
        current = current->next;
    }

    return current;
}

/*********************************BOTS**********************************/

//initialize bot list of type client with num_bots bots
void initBots(int num_bots, char *ip, client *head_bots, WINDOW *my_win) {
    for (int i = 0; i < num_bots; i++) {
        addBot(ip, head_bots, my_win);
    }
}

//create addBot without using addClient
void addBot(char *ip, client *head_bots, WINDOW *my_win) {
    player_position_t *p = (player_position_t*) malloc(sizeof(player_position_t));
    if (p == NULL) {
        perror("Error allocating memory for new bot");
        exit(-1);
    }

    client *newBot = (client*) malloc(sizeof(client));
    if (newBot == NULL) {
        perror("Error allocating memory for new bot");
        exit(-1);
    }

    strcpy(newBot->address, ip);
    new_bot_position(p, my_win);
    newBot->p = p;
    newBot->next = NULL;

    if (head_bots == NULL) {
        head_bots = newBot;
    } else {
        client *current = head_bots;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newBot;
    }

    return;
}

