#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/un.h>
#include "message.h"

struct sockaddr_un get_addr(char *path);
int unix_socket_init(const char *path);
int tcp_socket_init(char *ip, int port);
int tcp_connect(char *ip, int port);
void close_socket(int sfd);
int accept_connection(int sfd, struct sockaddr *addr, socklen_t *addr_len);
void send_message(int sfd, message *msg);
void receive_message(int sfd, message *msg);
#endif
