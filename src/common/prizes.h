#ifndef PRIZES_H
#define PRIZES_H

#include <stdbool.h>
#include <time.h>
#include <ncurses.h>

#include "defs.h"

typedef struct prize_pos{
    int x, y;
    int hp;
} prize_pos;

typedef struct prize{
    prize_pos *pr;

    struct prize *next;
} prize;

typedef struct list_to_array_prizes{
    prize_pos *array;
    int size;
} array_and_size_prizes;

//prize *head_prizes;
void addPrize(prize_pos *to_add, prize *head_prizes);
prize_pos *getPrizeByPos(int x, int y, prize *head_prizes);
void removePrize(prize_pos *to_remove, prize *head_prizes);
array_and_size_prizes *getPrizesArray(prize *head_prizes);
void freePrizeList(prize *head_prizes);
int getNumPrizes(prize *head_prizes);

void new_prize (WINDOW *my_win, prize_pos *prize);
void draw_prize(WINDOW *my_win, prize_pos *prize, bool delete);
prize_pos *init_prize(WINDOW *my_win);
time_t updatePrizes(WINDOW *my_win, time_t time0, prize *head_prizes);

#endif