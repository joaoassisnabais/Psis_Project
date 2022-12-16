#ifndef PRIZES_H
#define PRIZES_H

#include <stdbool.h>
#include <time.h>
#include <ncurses.h>
#include "defs.h"


typedef struct list_to_array_prizes{
    prize_pos *array;
    int size;
} array_and_size_prizes;

//prize *head_prizes;
<<<<<<< HEAD:src/common_backup/prizes.h
prize_pos *init_prize(WINDOW *my_win);
void addPrize(prize_pos *to_add, prize *head_prizes);
prize_pos *getPrizeByPos(int x, int y, prize *head_prizes);
=======

prize *getPrizeByPos(int x, int y, prize *head_prizes);
>>>>>>> master:src/common/prizes.h
void removePrize(prize_pos *to_remove, prize *head_prizes);
array_and_size_prizes *getPrizesArray(prize *head_prizes);
void freePrizeList(prize *head_prizes);
int getNumPrizes(prize *head_prizes);

void draw_prize(WINDOW *my_win, prize_pos *prize, bool delete);

#endif
