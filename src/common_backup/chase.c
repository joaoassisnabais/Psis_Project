#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "chase.h"
#include "prizes.h"
#include "list.h"
#include "message.h"

void parse_message(message msg, WINDOW *my_win, WINDOW *message_win){
    int i;
    //char *txt = msg.txt;
    player_position_t *players = msg.players;
    player_position_t *bots = msg.bots;
    prize_pos *prizes = msg.prizes;

    wclear(my_win); //might need to add box
    wclear(message_win);
    //wprintw(message_win, "%s", txt);
    
    //print players
    for(i = 0; i < msg.num_players; i++){
        mvwprintw(my_win, players[i].y, players[i].x, "%c", players[i].c);
    }
    //print bots
    for(i = 0; i < msg.num_bots; i++){
        mvwprintw(my_win, bots[i].y, bots[i].x, "%c", bots[i].c);
    }
    //print prizes
    for(i = 0; i < msg.num_prizes; i++){
        mvwprintw(my_win, prizes[i].y, prizes[i].x, "%c", prizes[i].hp);
    }

    wrefresh(my_win);
    wrefresh(message_win);
}

void init_window(WINDOW *my_win, WINDOW *message_win){
    initscr();		        /* Start curses mode */
    keypad(stdscr, TRUE);   /* Enable keyboard mapping */
	noecho();			    /* Don't echo() while we do getch */

    my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);

    message_win = newwin(5, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);
}

void draw_player(WINDOW *win, player_position_t * player, bool delete){
    int ch;
    if(delete){
        ch = ' ';
    }else{
        ch = player->c;
    }
    int p_x = player->x;
    int p_y = player->y;
    wmove(win, p_y, p_x);
    waddch(win,ch);
    wrefresh(win);
}


void move_player (player_position_t * player, int direction){

    if (direction == KEY_UP){
        if (player->y != 1){
            player->y--;
        }
    }
    if (direction == KEY_DOWN){
        if (player->y  != WINDOW_SIZE-2){
            player->y ++;
        }
    }
    
    if (direction == KEY_LEFT){
        if (player->x  != 1){
            player->x --;
        }
    }
    if (direction == KEY_RIGHT)
        if (player->x  != WINDOW_SIZE-2){
            player->x ++;
    }
}

player_position_t *init_client(WINDOW *my_win){

    player_position_t *p = (player_position_t *) malloc(sizeof(player_position_t));

    new_player_position(p, my_win); 
    draw_player(my_win, p, false);

    return p;
}
