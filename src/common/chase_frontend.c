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
            if(game_state->players[i].health>0){  
                wmove(game_screen.game_window, game_state->players[i].y, game_state->players[i].x);
                waddch(game_screen.game_window, game_state->players[i].c);
            }
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
        if(game_state->players[i].health>0){  
            wmove(game_screen.message_window, i+1, 1);
            waddch(game_screen.message_window, game_state->players[i].c);
            wprintw(game_screen.message_window, " %d", game_state->players[i].health);
        }
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

void kill_window(screen *game_screen){
    delwin(game_screen->game_window);
    delwin(game_screen->message_window);
    endwin();
}

