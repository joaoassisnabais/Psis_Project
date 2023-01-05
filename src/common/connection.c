#include <stdlib.h>
#include <sys/un.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include "connection.h"

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
    Close the socket
*/
void close_socket(int sfd){
    close(sfd);
}
