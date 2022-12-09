#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>

#include "server.h"
#include "list.h"
#include "chase.h"

int main(int argc, char **argv){
    int port;
    char ip[16];

    init_window();

    strcpy(ip,argv[1]);
    sscanf(argv[2], "%d", &port);
    server_loop(ip, port);
    
    exit(0);
}


void server_loop(char* addr, char* port){
    char in_msg[128], ip_address[20];
    void *msg = in_msg;
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    int udp_socket = udp_socket_init(addr, port);

    while (1)
    {    
        //Receive the message from the client
        if(recvfrom(udp_socket, &msg, MSG_LEN, 0, (struct sockaddr *) &clientAddr, &clientAddrLen) == -1){
            perror("Error receiving message");
            exit(-1);
        }; 

        strcpy(ip_address, inet_ntoa(clientAddr.sin_addr));
        run_processes(msg, ip_address);

        if(strcmp(msg, "no_reply") != 0){   /* If the client is disconnected or the message isn't listed, don't send anything */
            if(memcmp(msg, my_win, sizeof(my_win)) == 0){
                if (sendto(udp_socket, msg, sizeof(my_win), 0, (struct sockaddr *) &clientAddr, &clientAddrLen) != sizeof(msg) ){
                    perror("Full message wasn't sent");
                    exit(-1);
                }
            }else if (sendto(udp_socket, msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, &clientAddrLen) != sizeof(msg) ){
                perror("Full message wasn't sent");
                exit(-1);
            }
        }
            
    }

}

void run_processes(void *msg, char *ip_adress){
        
        char command[16];
        sscanf(msg, "%s", command);

        if (strcmp(command, "connect") == 0) {
            client *new = addClient(ip_adress, init_client(my_win), head_clients);
            strcpy(msg, "");
            sprintf(msg, "ball_info %d %d %c", new->p->x, new->p->y, new->p->c);
            return;

        } else if (strncmp(command, "move") == 0) {
            if(getClient(ip_adress,head_clients)->p->health <= 0){
                strcpy(msg, "dead");    /* Healt_0 message */
                removeClient(ip_adress, head_clients);
                return;
            }
            int move_key;
            sscanf(msg, "%*s %d", move_key);
            updatePosition(getClient(ip_adress, head_clients)->p, move_key);
            msg = my_win;
            return;

        } else if (strcmp(command, "disconnect") == 0) {
            removeClient(ip_adress, head_clients);
            strcpy(msg, "no_reply");
        } else {
            strcpy(msg, "no_reply");
        }
}


void init_window(){
    initscr();		    	/* Start curses mode */
    keypad(stdscr, TRUE);    /* Enable keyboard mapping */
	noecho();			    /* Don't echo() while we do getch */

    my_win = newwin(WINDOW_SIZE, WINDOW_SIZE, 0, 0);
    box(my_win, 0 , 0);	
	wrefresh(my_win);

    message_win = newwin(5, WINDOW_SIZE, WINDOW_SIZE, 0);
    box(message_win, 0 , 0);	
	wrefresh(message_win);
}
