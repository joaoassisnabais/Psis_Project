#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "prizes.h"
#include "chase.h"


prize *addPrize(prize_pos *to_add, prize *head_prizes) {
    prize *newPrize = (prize*) malloc(sizeof(prize));
    if (newPrize == NULL) {
        perror("Error allocating memory for new prize");
        exit(-1);
    }
    newPrize->pr;
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

    return newPrize;
}


//get prize by position
prize *getPrizeByPos(int x, int y, prize *head_prizes) {
    prize *current = head_prizes;

    if (current == NULL) {
        return NULL;
    }

    while (current != NULL && (current->pr->x != x && current->pr->y != y)) {
        current = current->next;
    }

    return current;
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
array_and_size *getPrizesArray(prize *head_prizes) {
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
    array_and_size *out = (array_and_size*) malloc(sizeof(array_and_size));
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

time_t updatePrizes(WINDOW *my_win, time_t time0){
    if (clock()-time0 >= 5 && getNumPrizes(head_prizes)<10){
        addPrize(init_prize(my_win), head_prizes);
        return clock();
    }
    return time0;
}

void new_prize (WINDOW *my_win, prize_pos *prize){
    prize_pos aux;
    prize->hp = 1 + (rand() % 5);
    char placeholder = NULL;
    while (placeholder != ' '){
        aux.x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
        aux.y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
        placeholder = mvwinch(my_win, aux.y, aux.x);
    }
    prize->x = aux.x;
    prize->y = aux.y;
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

prize_pos *init_prize(WINDOW *my_win){
    prize_pos *pr = (prize_pos *) malloc(sizeof(prize_pos));

    new_prize(my_win, pr); 
    draw_prize(my_win, &pr, false);

    return pr;
}