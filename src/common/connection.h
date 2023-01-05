#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/un.h>

struct sockaddr_un get_addr(char *path);
int unix_socket_init(const char *path);
int tcp_socket_init(char *ip, int port);
int tcp_connect(char *ip, int port);
void close_socket(int sfd);

#endif
