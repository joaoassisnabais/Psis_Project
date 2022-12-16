#ifndef UDP_H
#define UDP_H


int unix_socket_init(char *path);
struct sockaddr_un getAddr(char *path);
void send_msg(char *msg_txt, struct sockaddr_un servaddr);

#endif
