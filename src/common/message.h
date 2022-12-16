#ifndef MESSAGE_H
#define MESSAGE_H

#include "defs.h"
#include "chase_internal.h"

typedef struct message{
    char txt[MSG_TXT_SIZE];
    game state;
} message;

void create_message(message **msg, char *msg_txt, game *state);

#endif