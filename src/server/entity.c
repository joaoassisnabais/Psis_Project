#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "../common/udp.h"
#include "../common/chase.h"
#include "../common/defs.h"
#include "../common/list.h"
#include "../common/prizes.h"
#include "entity.h"

bool isPlayerCol(int x, int y, client *head_clients){
    client *aux = getClientByPos(x, y, head_clients);
    if(aux != NULL){
        if(aux->p->x == x && aux->p->y == y) return true;
    }
    return false;
}

bool isBotCol(int x, int y, client *head_bots){
    client *aux = getClientByPos(x, y, head_bots);
    if(aux != NULL){
        if(aux->p->x == x && aux->p->y == y) return true;
    }
    return false;
}

bool isPrizeCol(int x, int y, prize *head_prizes){
    prize *aux = getPrizeByPos(x, y, head_prizes);
    if(aux != NULL){
        if(aux->pr->x == x && aux->pr->y == y) return true;
    }
    return false;
}

bool isEmpty(int x, int y, client *head_clients, client *head_bots, prize *head_prizes){
    if(isPlayerCol(x, y, head_clients) || isBotCol(x, y, head_bots) || isPrizeCol(x, y, head_prizes)){
        return false;
    }
    return true;
}

void new_player_position (player_position_t *player, WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes){
    player_position_t aux;
    player->health = MAX_HP;
    bool is_empty = false;
    while (!is_empty){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        is_empty = isEmpty(aux.x, aux.y, head_clients, head_bots, head_prizes);
    }
    player->x = aux.x;
    player->y = aux.y;
}

<<<<<<< HEAD
=======
void new_bot_position (player_position_t *bot, WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes){
    player_position_t aux;
    bot->c = '*';
    bot->health = 99999;
    bool is_empty = false;
    while (!is_empty){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        is_empty = isEmpty(aux.x, aux.y, head_clients, head_bots, head_prizes);
    }
    bot->x = aux.x;
    bot->y = aux.y;
}

void new_prize(prize_pos *p, client *head_clients, client *head_bots, prize *head_prizes){
    prize_pos aux;
    p->hp = 1 + (rand() % 5);
    bool is_empty = false;
    while (!is_empty){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generatesfalsea random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        is_empty = isEmpty(aux.x, aux.y, head_clients, head_bots, head_prizes);
    }
    p->x = aux.x;
    p->y = aux.y;
}

prize_pos *init_prize(WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes){
    prize_pos *p = (prize_pos *) malloc(sizeof(prize_pos));

    new_prize(p, head_clients, head_bots, head_prizes); 
    draw_prize(my_win, p, false);

    return p;
}

player_position_t *init_client(WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes){

    player_position_t *p = (player_position_t *) malloc(sizeof(player_position_t));

    new_player_position(p, my_win, head_clients, head_bots, head_prizes); 
    draw_player(my_win, p, false);

    return p;
}

//initialize bot list of type client with num_bots bots
void initBots(int num_bots, char *ip, WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes) {
    for (int i = 0; i < num_bots; i++) {
        addBot(ip, my_win, head_clients, head_bots, head_prizes);
    }
}

//create addBot without using addClient
void addBot(char *ip, WINDOW *my_win, client *head_bots, client *head_clients, prize *head_prizes) {
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
    new_bot_position(p, my_win, head_clients, head_bots, head_prizes);
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

void addPrize(prize_pos *to_add, prize **head_prizes) {
    prize *newPrize = (prize*) malloc(sizeof(prize));
    if (newPrize == NULL) {
        perror("Error allocating memory for new prize");
        exit(-1);
    }
    newPrize->pr = to_add;
    newPrize->next = NULL;

    if (*head_prizes == NULL) {
        *head_prizes = newPrize;
    } else {
        prize *current = *head_prizes;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newPrize;
    }

    return;
}

time_t updatePrizes(WINDOW *my_win, time_t time0, client *head_client, client *head_bots, prize *head_prizes){
    if (clock()-time0 >= 5 && getNumPrizes(head_prizes)<10){
        addPrize(init_prize(my_win, head_client, head_bots, head_prizes), &head_prizes);
        return clock();
    }
    return time0;
}
>>>>>>> master
