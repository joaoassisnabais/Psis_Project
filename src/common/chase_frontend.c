#include "chase_frontend.h"
#include "defs.h"

void render(screen game_screen, game *game_state) {
    wclear(game_screen.game_window);
    wclear(game_screen.message_window);

    for(int i = 0; i < game_state->num_players; i++){
        mvwprintw(game_screen.game_window, game_state->players[i].y, game_state->players[i].x, "%c", game_state->players[i].c);
    }
    //print bots
    for(int i = 0; i < game_state->num_bots; i++){
        mvwprintw(game_screen.game_window, game_state->bots[i].y, game_state->bots[i].x, "%c", game_state->bots[i].c);
    }
    //print prizes
    for(int i = 0; i < game_state->num_prizes; i++){
        mvwprintw(game_screen.game_window, game_state->prizes[i].y, game_state->prizes[i].x, "%c", game_state->prizes[i].hp);
    }

    wrefresh(game_screen.game_window);
    wrefresh(game_screen.message_window);
}

void init_window(screen *game_screen){
    initscr();		        /* Start curses mode */
    keypad(stdscr, TRUE);   /* Enable keyboard mapping */
	noecho();			    /* Don't echo() while we do getch */

    game_screen->game_window = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(game_screen->game_window, 0 , 0);	
	wrefresh(game_screen->game_window);

    game_screen->message_window = newwin(5, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(game_screen->message_window, 0 , 0);	
	wrefresh(game_screen->message_window);
}
