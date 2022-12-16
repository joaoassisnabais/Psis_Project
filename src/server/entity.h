#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <time.h>
#include <ncurses.h>

#include "../common/chase_internal.h"

char bot_address[108]; 

#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

#include "../common/defs.h"
#include "../common/chase_internal.h"
#include "entity.h"

char bot_address[108];
char player_address[10][108];

player_position_t *getClientByPos(int x, int y, game *state);
bool isPlayerCol(int x, int y, game *state);
bool isPrizeCol(int x, int y, game *state);
bool isEmpty(int x, int y, game *state);
player_position_t new_player_position (game *state);
player_position_t new_bot_position (game *state);
void addPlayer(game *state, char *address);
void rmPlayer(game *state, player_position_t* player);
void initBots(int num_bots, char *address, game *state);
void addBot(char *address, game *state);
prize_pos new_prize(game *state);
void addPrize(game *state);
void initPrizes(game *state, int num_prizes);
time_t updatePrizes(time_t time0, game *state);
player_position_t *getPlayerByAddr(char *address, game *state);
prize_pos getPrizeByPos(int x, int y, game *state);
void rmPrizeByPos(int x, int y, game *state);
void rmPlayerByAddr(game *state, char *address);
#endif
