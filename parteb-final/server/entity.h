#ifndef ENTITY_H
#define ENTITY_H

#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "../common/chase_internal.h"
#include "../common/defs.h"

player_position_t *getClientByPos(int x, int y, game *state);
bool isPlayerCol(int x, int y, game *state);
bool isPrizeCol(int x, int y, game *state);
bool isEmpty(int x, int y, game *state);
player_position_t new_player_position (game *state);
player_position_t new_bot_position (game *state);
void addPlayer(game *state, int fd);
void rmPlayer(game *state, player_position_t* player);
void initBots(int num_bots, game *state);
void addBot(game *state);
prize_pos new_prize(game *state);
void addPrize(game *state);
void initPrizes(game *state, int num_prizes);
bool timeout_dead(time_t time0);
player_position_t *getPlayerByAddr(int fd, game *state);
prize_pos getPrizeByPos(int x, int y, game *state);
void rmPrizeByPos(int x, int y, game *state);
void rmPlayerByAddr(game *state, int fd);
bool PlayerAuth(int fd, int max_players);
int getPlayerFd(game *state, player_position_t *player);

#endif
