#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>


int udp_socket_init(char * address, char * port){
    struct addrinfo hints,*result;
    int sfd;

    sfd=socket(AF_INET,SOCK_DGRAM,0);   /* Open UDP socket */
    if(sfd == -1){ 
        perror("");
        exit(-1);
    }

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      /* Allow IPv4 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;    /* Receiving */
    hints.ai_protocol = 0;          /* Any protocol */

    //Get the address info
    if (getaddrinfo(address, port, &hints, &result) != 0) {
        perror("");
        exit(-1);
    }

    //Bind the socket to the port
    if (bind(sfd, result->ai_addr, result->ai_addrlen) != 0) {
        perror("");
        exit(-1);
    }
    freeaddrinfo(result);
    
    return sfd;
}

struct sockaddr getAddr(char *ip, char *port){
    struct addrinfo hints,*result;
    struct sockaddr addr;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      /* Allow IPv4 */
    hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
    hints.ai_protocol = 0;          /* Any protocol */

    //Get the address info
    if (getaddrinfo(ip, port, &hints, &result) != 0) {
        perror("");
        exit(-1);
    }

    addr = *result->ai_addr;
    freeaddrinfo(result);
    
    return addr;
}

