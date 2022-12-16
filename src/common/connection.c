#include <stdlib.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include "connection.h"

struct sockaddr_un get_addr(char *path){
    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    return addr;
}

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
