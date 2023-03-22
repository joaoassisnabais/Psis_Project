#ifndef MESSAGE_H
#define MESSAGE_H

#include "defs.h"
#include "chase_internal.h"

#pragma pack(push, 1)
typedef struct message{
    char txt[MSG_TXT_SIZE];
    game state;
} message;
#pragma pack(pop)

void create_message(message *msg, char *msg_txt, game *state);
void alloc_players(message *msg);
void free_players(message *msg);
void copy_players(message *msg, game *state);

#endif
