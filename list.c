#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>

#include "list.h"
#include "chase.h"

client * addClient(char *ip_adress, player_position_t *p, client *head_clients) {
    client *newClient = (client*) malloc(sizeof(client)); 
    if( newClient == NULL) {
        perror("Error allocating memory for new client");
        exit(-1);
    }
    strcpy(newClient-> ip_adress, ip_adress);
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
void removeClient(char *ip_adress, client *head_clients) {
    client *current = head_clients;
    client *previous = NULL;

    if (current == NULL) {  //no list
        return;
    }

    //runs until it finds the client or the end of the list
    while (current != NULL && strcmp(current->ip_adress, ip_adress) != 0) { 
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

client *getClient(char *ip_adress, client *head_clients) {
    client *current = head_clients;

    if (current == NULL) {
        return NULL;
    }

    while (current != NULL && strcmp(current->ip_adress, ip_adress) != 0) {
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