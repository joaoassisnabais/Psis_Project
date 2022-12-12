#ifndef udp_h
#define udp_h

int udp_socket_init(char * host, char * port);
struct sockaddr getAddr(char *ip, char *port);

#endif