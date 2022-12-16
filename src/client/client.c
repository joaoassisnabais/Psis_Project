#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ncurses.h>    
#include <string.h>
#include <stdlib.h>

#include "../common/defs.h"
#include "../common/connection.h"
#include "../common/message.h"
#include "../common/chase_frontend.h"

int dgram_socket;
bool serv_full=false;
game game_state;
screen game_screen = {NULL, NULL};
player_position_t player;

// Parse the messages
// ----

void parse_ball_info(message *msg) {
    sscanf(msg->txt, "%*s %d %d %c", &player.x, &player.y, &player.c);
    player.health = MAX_HP;
    game_state = msg->state;
}

void parse_field_status(message *msg) {
    sscanf(msg->txt, "%*s %d %d %d", &player.x, &player.y, &player.health);
    game_state = msg->state;
}

void parse_dead(message *msg) {
    player.health = 0;
    game_state = msg->state;
}
// ----

/*
    Sends messages to the server
*/
void send_msg(const char *msg_txt, struct sockaddr_un servaddr){
    char command[16];
    message msg;
    strncpy(msg.txt, msg_txt, MSG_TXT_SIZE);
    sscanf(msg.txt, "%s", command);

    if(strcmp(command, "connect") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "move") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));

    }else if(strcmp(command, "disconnect") == 0){
        sendto(dgram_socket, &msg, sizeof(msg), 0, (struct sockaddr *) &servaddr, sizeof(servaddr));
    }
}

/*
    Receives game updates
*/
void receive(){ // Receive a message, parse it and update state
    message msg;
    char command[16];

    if(recv(dgram_socket, &msg, sizeof(msg), 0) == 0){
        perror("Error receiving message, message is empty");
        exit(-1);
    }

    sscanf(msg.txt, "%s", command);
    
    if(strcmp(command, "ball_info") == 0){
        parse_ball_info(&msg);
    }else if(strcmp(command, "field_status") == 0){
        parse_field_status(&msg);
    }else if(strcmp(command, "dead") == 0){
        parse_dead(&msg);
    }else if (strcmp(command, "full") == 0) {
        serv_full = true;
    }
}

/*
    Pressed key to direction translation
*/
dir key_to_dir(int key) {
    switch (key) {
        case KEY_LEFT:
            return DIR_LEFT;
        case KEY_RIGHT:
            return DIR_RIGHT;
        case KEY_UP:
            return DIR_UP;
        case KEY_DOWN:
            return DIR_DOWN;
        default:
            return DIR_NONE;
    }
}

/*
    Loop to send keys to server and get game updates
*/
void client_loop(struct sockaddr_un serv_addr){
    char msg[100];
    
    send_msg("connect", serv_addr);
    receive();
    if (serv_full) return;

    int key=-1;
    while(key != 'q' && key != 27){
        key = getch();

        dir direction = key_to_dir(key);

        if (direction != DIR_NONE) {
            wprintw(game_screen.game_window, "key: %d\n", key);
            sprintf(msg, "move %d", direction);
            send_msg(msg, serv_addr);
            receive();
        }

        render(game_screen, &game_state);

        if(player.health <= 0){
            break;
        }
    }
    if(key == 'q' || key == 27) send_msg("disconnect", serv_addr);
}

int main(int argc, char **argv ){;
    if (argc < 2) {
        printf("Usage: ./chase-client <server_address>\n");
        exit(0);
    }

    char client_addr[108];

    init_window(&game_screen);
    cbreak();				/* Line buffering disabled	*/

    sprintf(client_addr, "/tmp/client%d", getpid());
    dgram_socket = unix_socket_init(client_addr);

    client_loop(get_addr(argv[1]));

    kill_window(&game_screen);
    unlink(client_addr);
    printf("\nKicked out!\n");
    exit(0);
}
