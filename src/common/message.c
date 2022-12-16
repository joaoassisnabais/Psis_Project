#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "defs.h"

void create_message(message **msg, char *msg_txt, game *state){ 
    if (msg_txt != NULL) strncpy((*msg)->txt, msg_txt, MSG_TXT_SIZE);
    if (state != NULL) (*msg)->state = *state;
}