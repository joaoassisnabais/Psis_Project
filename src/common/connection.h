#ifndef CONNECTION_H
#define CONNECTION_H

#include <sys/un.h>

struct sockaddr_un get_addr(char *path);
int unix_socket_init(const char *path);

#endif
