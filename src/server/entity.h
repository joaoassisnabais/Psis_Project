#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <time.h>
#include <ncurses.h>

#include "../common/prizes.h"
#include "../common/list.h"

void new_player_position (player_position_t *player, WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes);
void new_bot_position (player_position_t *bot, WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes);

bool isEmpty(int x, int y, client *head_clients, client *head_bots, prize *head_prizes);
bool isPlayerCol(int x, int y, client *head_clients);
bool isBotCol(int x, int y, client *head_bots);
bool isPrizeCol(int x, int y, prize *head_prizes);

void new_prize (prize_pos *p, client *head_clients, client *head_bots, prize *head_prizes);
void addPrize(prize_pos *to_add, prize **head_prizes);
prize_pos *init_prize(WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes);

player_position_t *init_client(WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes);

void initBots(int num_bots, char *ip, WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes);
void addBot(char *ip, WINDOW *my_win, client *head_clients, client *head_bots, prize *head_prizes);
time_t updatePrizes(WINDOW *my_win, time_t time0, client *head_clients, client *head_bots, prize *head_prizes);

#endif