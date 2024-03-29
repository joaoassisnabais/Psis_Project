#ifndef CHASE_INTERNAL_H
#define CHASE_INTERNAL_H

typedef struct player_position_t{
    int x, y, health;
    char c;
} player_position_t;

typedef struct prize_pos{
    int x, y;
    int hp;
} prize_pos;

#pragma pack(push, 1)
typedef struct game {
    int num_players;
    int num_bots;
    int num_prizes;
    player_position_t bots[10];
    prize_pos prizes[10];
    player_position_t *players;
} game;
#pragma pack(pop)

#endif
