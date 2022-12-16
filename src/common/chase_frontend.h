#ifndef CHASE_FRONTEND_H
#define CHASE_FRONTEND_H

#include <ncurses.h>
#include "chase_internal.h"

typedef struct {
    WINDOW *game_window, *message_window;
} screen;

void init_window(screen *game_screen);
void render(screen game_screen, game *state);

#endif
