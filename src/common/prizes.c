#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "prizes.h"
#include "chase.h"


void addPrize(prize_pos *to_add, prize *head_prizes) {
    prize *newPrize = (prize*) malloc(sizeof(prize));
    if (newPrize == NULL) {
        perror("Error allocating memory for new prize");
        exit(-1);
    }
    newPrize->pr = to_add;
    newPrize->next = NULL;

    if (head_prizes == NULL) {
        head_prizes = newPrize;
    } else {
        prize *current = head_prizes;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newPrize;
    }

    return;
}


//get prize by position
prize_pos *getPrizeByPos(int x, int y, prize *head_prizes) {
    prize *current = head_prizes;

    if (current == NULL) {
        return NULL;
    }

    while (current != NULL && (current->pr->x != x && current->pr->y != y)) {
        current = current->next;
    }

    return current->pr;
}

//remove prize_pos from prize struct
void removePrize(prize_pos *to_remove, prize *head_prizes) {
    prize *current = head_prizes;
    prize *previous = NULL;

    if (current == NULL) {  //no list
        return;
    }

    //runs until it finds the prize or the end of the list
    while (current != NULL && (current->pr->x != to_remove->x && current->pr->y != to_remove->y)) { 
        previous = current;
        current = current->next;
    }

    //prize not found
    if (current == NULL) {
        return;
    }

    //prize is first in list (1) or in the middle/end of list (2)
    if (previous == NULL) {
        head_prizes = current->next;
    } else {
        previous->next = current->next;
    }

    free(current);
}

//get number of prizes
int getNumPrizes(prize *head_prizes) {
    prize *current = head_prizes;
    int i = 0;

    while (current != NULL) {
        current = current->next;
        i++;
    }

    return i;
}

//array and size for prizes
array_and_size_prizes *getPrizesArray(prize *head_prizes) {
    prize_pos *array = (prize_pos*) malloc(sizeof(prize_pos) * 10);
    if (array == NULL) {
        perror("Error allocating memory for prize_pos array");
        exit(-1);
    }

    prize *current = head_prizes;
    int i = 0;

    while (current != NULL) {
        array[i] = *current->pr;
        current = current->next;
        i++;
    }
    array_and_size_prizes *out = (array_and_size_prizes*) malloc(sizeof(array_and_size_prizes));
    out->array = array;
    out->size = i;
    
    return out;
}



//free all memory allocated for the list and delete all prizes
void freePrizeList(prize *head_prizes) {
    prize *current = head_prizes;
    prize *next = NULL;

    while (current != NULL) {
        next = current->next;
        free(current->pr);
        free(current);
        current = next;
    }
}

time_t updatePrizes(WINDOW *my_win, time_t time0, prize *head_prizes){
    if (clock()-time0 >= 5 && getNumPrizes(head_prizes)<10){
        addPrize(init_prize(my_win), head_prizes);
        return clock();
    }
    return time0;
}

void draw_prize(WINDOW *my_win, prize_pos *prize, bool delete){
    int hp;
    if(delete){
        hp = ' ';
    }else{
        hp = prize->hp;
    }
    int p_x = prize->x;
    int p_y = prize->y;
    wmove(my_win, p_y, p_x);
    waddch(my_win, hp);
    wrefresh(my_win);
}
