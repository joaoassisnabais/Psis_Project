#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "message.h"
#include "defs.h"

message read_message(void *buffer){
    char txt_buffer[MSG_TXT_SIZE], win_buffer[MSG_WIN_SIZE];
    message msg;

    if(buffer ==  NULL){
        perror("Buffer from incoming message is NULL");
        exit(-1);
    }

    strncpy(&txt_buffer, &buffer, MSG_TXT_SIZE);
    memcpy(&win_buffer, &buffer + MSG_TXT_SIZE, MSG_WIN_SIZE);

    strncpy(msg.txt, txt_buffer, MSG_TXT_SIZE);
    memcpy((void *)&msg.win, win_buffer, MSG_WIN_SIZE);

    return msg;
}
