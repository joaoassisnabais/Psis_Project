#include <stdlib.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

#include "connection.h"
#include "chase_internal.h"
#include "message.h"

/*
    Gets struc sockaddr_un from path
*/
struct sockaddr_un get_addr(char *path){
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    return addr;
}

/*
    Opens and binds a socket to the given path
*/
int unix_socket_init(const char *path){
    struct sockaddr_un addr;
    int sfd;

    sfd=socket(AF_UNIX,SOCK_DGRAM,0); // Open datagram socket
    if(sfd == -1){ 
        perror("cant create socket on server");
        exit(-1);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    // int option = 1;
    // setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));
    unlink(path);   //Delete the socket if it already exists

    //Bind the socket to the port
    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_un)) != 0) {
        unlink(path);
        perror("can't bind socket");
        exit(-1);
    }
    
    return sfd;
}

/********************************-TCP-********************************/

/*
    Opens and binds a socket to the given ip and port
*/
int tcp_socket_init(char *ip, int port){
    int sfd;
    struct sockaddr_in addr;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1){
        perror("cant create socket on server");
        exit(-1);
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) != 0) {
        perror("can't bind socket");
        exit(-1);
    }

    if (listen(sfd, 20) != 0) {
        perror("listen failed");
        exit(-1);
    }

    return sfd;
}

/*
    Connects to the given ip and port
*/
int tcp_connect(char *ip, int port){
    int sfd;
    struct sockaddr_in addr;

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1){
        perror("cant create socket on client");
        exit(-1);
    }

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (connect(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) != 0) {
        perror("can't connect to server");
        exit(-1);
    }

    return sfd;

}

/*
    Accepts a connection
*/
int accept_connection(int sfd, struct sockaddr *addr, socklen_t *addr_len){
    int cfd;
    cfd = accept(sfd, addr, addr_len);
    if(cfd == -1){
        perror("can't accept connection");
        exit(-1);
    }
    return cfd;
}

/*
    Close the socket
*/
void close_socket(int sfd){
    close(sfd);
}

/*
    Send a message to the given socket
*/
void send_message(int fd, message *msg){
    write(fd, msg->txt, sizeof(msg->txt));
    write(fd, &msg->state.num_bots, sizeof(msg->state.num_bots));
    write(fd, &msg->state.num_prizes, sizeof(msg->state.num_prizes));
    write(fd, &msg->state.num_players, sizeof(msg->state.num_players));
    write(fd, &msg->state.prizes, sizeof(msg->state.prizes));
    write(fd, &msg->state.bots, sizeof(msg->state.bots));
    if(msg->state.players != NULL) write(fd, msg->state.players, msg->state.num_players * sizeof(player_position_t));
}

/*
    Receive a message from a given socket
*/
void receive_message(int fd, message *msg){
    if(read(fd, msg->txt, sizeof(msg->txt)) == 0){
        strcpy(msg->txt, "quit");
    }
    if(errno == EBADF){
        strcpy(msg->txt, "quit");
    }
    read(fd, &msg->state.num_bots, sizeof(msg->state.num_bots));
    read(fd, &msg->state.num_prizes, sizeof(msg->state.num_prizes));
    read(fd, &msg->state.num_players, sizeof(msg->state.num_players));
    read(fd, &msg->state.prizes, sizeof(msg->state.prizes));
    read(fd, &msg->state.bots, sizeof(msg->state.bots));
    if (msg->state.num_players != 0){
        alloc_players(msg);
        read(fd, msg->state.players, msg->state.num_players * sizeof(player_position_t));
    }
}