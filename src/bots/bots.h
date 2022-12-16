#ifndef BOTS_H
#define BOTS_H

#include <sys/un.h>

int num_bots;
int dgram_socket;

void bot_loop(struct sockaddr_un serv_addr);
void send_msg(char *msg_txt, struct sockaddr_un servaddr);

#endif