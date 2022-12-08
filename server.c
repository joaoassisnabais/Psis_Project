#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>

#include "server.h"
#include "chase.h"

client *head_clients = NULL;
WINDOW *my_win = NULL;
WINDOW *message_win = NULL;

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
    char msg[128];
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

        run_processes(msg, inet_ntoa(clientAddr.sin_addr));
        

        //Send the message back to the client (DONT FORGET TO CHANGE MESSAGE)
        if (sendto(udp_socket, msg, sizeof(msg), 0, (struct sockaddr *) &clientAddr, &clientAddrLen) != sizeof(msg) ){
            perror("Full message wasn't sent");
            exit(-1);
        }
    }

}

void addClient(char *ip_adress, player_position_t *p) {
    client *newClient = (client*) malloc(sizeof(client)); 
    strcpy(newClient-> ip_adress, ip_adress);
    newClient->health = 10;
    newClient->p = p;
    newClient->next = NULL;

    if (head_clients == NULL) {
        head_clients = newClient;
        newClient->p->c = 'A';
    } else {
        client *current = head_clients;
        while (current->next != NULL) {
            current = current->next;
        }
        newClient->p->c = current->p->c + 1;
        current->next = newClient;
    }
}

//remove client from list
void removeClient(char *ip_adress) {
    client *current = head_clients;
    client *previous = NULL;

    if (current == NULL) {  //no list
        return;
    }

    //runs until it finds the client or the end of the list
    while (current != NULL && strcmp(current->ip_adress, ip_adress) != 0) { 
        previous = current;
        current = current->next;
    }

    //client not found
    if (current == NULL) {
        return;
    }

    //client is first in list (1) or in the middle/end of list (2)
    if (previous == NULL) {
        head_clients = current->next;
    } else {
        previous->next = current->next;
    }

    free(current->p);
    free(current);
}

void run_processes(char *msg, char *ip_adress){
        

        if (strcmp(msg, "connect") == 0) {
            addClient(ip_adress, init_client(my_win));
            strcpy(msg, "ball_info ");

        } else if (strcmp(msg, "moove") == 0) {
            //moove_player();
            strcpy(msg, "moove");
        } else if (strcmp(msg, "draw") == 0) {
            //draw_player();
            strcpy(msg, "draw");
        } else if (strcmp(msg, "delete") == 0) {
            //delete_player();
            strcpy(msg, "delete");
        } else {
            strcpy(msg, "error");
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
