#include <stdlib.h>
#include <stdbool.h>
#include <ncurses.h>

#include "list.h"
#include "chase.h"
#include "message.h"
#include "server.h"

void render_message(message msg, WINDOW *my_win, WINDOW *message_win){
    int i;
    char *txt = msg.txt;
    player_position_t *players = msg.players;
    player_position_t *bots = msg.bots;
    player_position_t *prizes = msg.prizes;

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
        mvwprintw(my_win, prizes[i].y, prizes[i].x, "%c", prizes[i].c);
    }

    wrefresh(my_win);
    wrefresh(message_win);
}

void init_window(){
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

void new_player_position (player_position_t *player){
    player->x = 1 + (rand() % (WINDOW_SIZE-3)); /* generates a random number between 1 and WINDOW_SIZE (not counting the edge) */
    player->y = 1 + (rand() % (WINDOW_SIZE-3)); /* potato potato */
    player->c = NULL;
    player->health = MAX_HP;
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

void updatePosition(player_position_t *player, int direction){
    player_position_t aux = *player;
    move_player(&aux, direction);

    char placeholder = mvwinch(my_win, aux.y, aux.x);
    if (placeholder == ' ' || placeholder == player->c){    /*moves into empty space or the wall*/
        draw_player(my_win, player, true);
        player->x = aux.x;
        player->y = aux.y;
        draw_player(my_win, player, false);
    }else if(placeholder<='5' && placeholder>='1'){         /*moves into a health pack*/
        draw_player(my_win, player, true);
        player->x = aux.x;
        player->y = aux.y;
        player->health+=placeholder-'0';
        draw_player(my_win, player, false);
    }else if(placeholder=='A' || placeholder=='B' || placeholder=='C' || placeholder=='D' || placeholder=='E' ||
             placeholder=='F' || placeholder=='G' || placeholder=='H' || placeholder=='I' || placeholder=='J'){     /*moves into another player*/
        player->health++;
        getClientByChar(placeholder, head_clients)->p->health--;
    }

    if(player->health>MAX_HP){
        player->health=MAX_HP;
    }
}


player_position_t *init_client(WINDOW *my_win){

    player_position_t *p = (player_position_t *) malloc(sizeof(player_position_t));

    new_player_position(p); 
    draw_player(my_win, &p, false);

    return p;
}
