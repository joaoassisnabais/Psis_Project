#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "udp.h"
#include "chase.h"
#include "entity.h"
#include "defs.h"
#include "list.h"
#include "prizes.h"

bool isPlayerCol(int x, int y, client *head_clients){
    if(getClientByPos(x, y, head_clients) != NULL) return false;
    return true;
}

bool isBotCol(int x, int y, client *head_bots){
    if(getClientByPos(x, y, head_bots) != NULL) return false;
    return true;
}

bool isPrizeCol(int x, int y, prize_pos *head_prizes){
    if(getPrizeByPos(x, y, head_prizes) != NULL) return false;
    return true;
}

bool isEmpty(int x, int y, client *head_clients, client *head_bots, prize_pos *head_prizes){
    if(isPlayerCol(x, y, head_clients) && isBotCol(x, y, head_bots) && isPrizeCol(x, y, head_prizes)) return true;
    return false;
}

void new_player_position (player_position_t *player, WINDOW *my_win){
    player_position_t aux;
    player->health = MAX_HP;
    char placeholder = 'a';
    while (placeholder != ' '){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        placeholder = mvwinch(my_win, aux.y, aux.x);
    }
    player->x = aux.x;
    player->y = aux.y;
}

void new_bot_position (player_position_t *bot, WINDOW *my_win){
    player_position_t aux;
    bot->c = '*';
    bot->health = 99999;
    char placeholder = 'a';
    while (placeholder != ' '){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        placeholder = mvwinch(my_win, aux.y, aux.x);
    }
    bot->x = aux.x;
    bot->y = aux.y;
}

void new_prize (WINDOW *my_win, prize_pos *prize, client *head_clients, client *head_bots, prize *head_prizes){
    prize_pos aux;
    prize->hp = 1 + (rand() % 5);
    bool is_empty = false;
    while (!is_empty){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        is_empty = isEmpty(aux.x, aux.y, head_clients, head_bots, head_prizes);
    }
    prize->x = aux.x;
    prize->y = aux.y;
}

prize_pos *init_prize(WINDOW *my_win){
    prize_pos *pr = (prize_pos *) malloc(sizeof(prize_pos));

    new_prize(my_win, pr, NULL, NULL, NULL); 
    draw_prize(my_win, pr, false);

    return pr;
}

player_position_t *init_client(WINDOW *my_win){

    player_position_t *p = (player_position_t *) malloc(sizeof(player_position_t));

    new_player_position(p, my_win); 
    draw_player(my_win, p, false);

    return p;
}