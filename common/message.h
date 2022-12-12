#include <ncurses.h>
#include "defs.h"


typedef struct message{
    char txt[MSG_TXT_SIZE];
    WINDOW win;
} message;

message read_message(void *buffer);