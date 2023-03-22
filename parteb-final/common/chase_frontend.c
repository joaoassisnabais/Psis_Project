#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "chase_frontend.h"
#include "defs.h"

/*
    Renders the screen from the game state
*/
void render(screen game_screen, game *game_state) {
    wclear(game_screen.game_window);
    box(game_screen.game_window, 0 , 0);

    wclear(game_screen.message_window);
    box(game_screen.message_window, 0 , 0);

    //print players
    for(int i = 0; i < game_state->num_players; i++){
        if(&game_state->players[i] != NULL){
            wmove(game_screen.game_window, game_state->players[i].y, game_state->players[i].x);
            waddch(game_screen.game_window, game_state->players[i].c);
        }
    }   
    //print bots
    for(int i = 0; i < game_state->num_bots; i++){
        wmove(game_screen.game_window, game_state->bots[i].y, game_state->bots[i].x);
        waddch(game_screen.game_window, game_state->bots[i].c);
    }
    //print prizes
    for(int i = 0; i < game_state->num_prizes; i++){
        wmove(game_screen.game_window, game_state->prizes[i].y, game_state->prizes[i].x);
        waddch(game_screen.game_window, game_state->prizes[i].hp + '0');
    }

    //print players char and health to message window
    for(int i = 0; i < game_state->num_players; i++){ 
        wmove(game_screen.message_window, i+1, 1);
        waddch(game_screen.message_window, game_state->players[i].c);
        if(game_state->players[i].health > 0) wprintw(game_screen.message_window, " %d", game_state->players[i].health);
        else wprintw(game_screen.message_window, " DEAD");
    }

    wrefresh(game_screen.game_window);
    wrefresh(game_screen.message_window);
}

/*
    Initializes the screen
*/
void init_window(screen *game_screen){
    initscr();		        /* Start curses mode */
    keypad(stdscr, TRUE);   /* Enable keyboard mapping */
	noecho();			    /* Don't echo() while we do getch */

    game_screen->game_window = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(game_screen->game_window, 0 , 0);	
	wrefresh(game_screen->game_window);

    game_screen->message_window = newwin(14, WINDOW_SIZE, 0, WINDOW_SIZE);
    box(game_screen->message_window, 0 , 0);	
	wrefresh(game_screen->message_window);
}

/*
    Kills the screen
*/
void kill_window(screen *game_screen){
    delwin(game_screen->game_window);
    delwin(game_screen->message_window);
    endwin();
}

//creates an initial window intructing the player to press any key to start
void start_window(screen *game_screen, WINDOW *start_window){
    wclear(game_screen->game_window);
    wclear(game_screen->message_window);

    static char msg[64]="Press any key to start";
    int size = round(strlen(msg)/2);;
 
    box(start_window, '#' , '#');
    
    wmove(game_screen->game_window, WINDOW_SIZE/2, WINDOW_SIZE/2 - size);
    wprintw(game_screen->message_window, msg, NULL);
    
    wrefresh(start_window);
    wrefresh(game_screen->game_window);
    wrefresh(game_screen->message_window);
}


void continue_window(screen *game_screen){
    wclear(game_screen->game_window);
    wclear(game_screen->message_window);

    static char msg[64]="#Press any \n#key to continue \n\n#Dead in 10 seconds";
    int size = round(strlen(msg)/2);
    wmove(game_screen->game_window, WINDOW_SIZE/2, WINDOW_SIZE/2 - size);
    wprintw(game_screen->game_window, msg, NULL);

    wrefresh(game_screen->game_window);
    wrefresh(game_screen->message_window);
}





















