#include <sys/types.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ncurses.h>    
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include "../common/defs.h"
#include "../common/connection.h"
#include "../common/message.h"
#include "../common/chase_frontend.h"

int serv_socket;
bool serv_full=false;
game game_state;
screen game_screen = {NULL, NULL};
player_position_t player;
bool dead=false;

pthread_mutex_t state_mutex = PTHREAD_MUTEX_INITIALIZER;


void exit_routine(){
    kill_window(&game_screen);
    close(serv_socket);
    printf("Goodbye!\n");
    exit(0);
}

/*
    Copy the game state from the message to the game state
*/
void copy_server_state(game *src){
    game_state.num_bots=src->num_bots;
    game_state.num_players=src->num_players;
    game_state.num_prizes=src->num_prizes;
    for(int i=0; i<src->num_bots; i++){
        game_state.bots[i]=src->bots[i];
    }
    for(int i=0; i<src->num_prizes; i++){
        game_state.prizes[i]=src->prizes[i];
    }
    if(game_state.players != NULL) free(game_state.players);
    game_state.players = (player_position_t *) malloc(sizeof(player_position_t) * src->num_players);
    memcpy(game_state.players, src->players, sizeof(player_position_t) * src->num_players);
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

// Parse the messages
// ----
//need to free the message game state and allocate new memmory for it in the real game state

void parse_ball_info(message *msg) {
    sscanf(msg->txt, "%*s %d %d %c", &player.x, &player.y, &player.c);
    player.health = MAX_HP;
    pthread_mutex_lock(&state_mutex);
    copy_server_state(&msg->state);
    pthread_mutex_unlock(&state_mutex);
}

void parse_field_status(message *msg) {
    sscanf(msg->txt, "%*s %d %d %d", &player.x, &player.y, &player.health);
    pthread_mutex_lock(&state_mutex);
    copy_server_state(&msg->state);
    pthread_mutex_unlock(&state_mutex);
}

void parse_dead(message *msg) {
    player.health = 0;
    pthread_mutex_lock(&state_mutex);
    copy_server_state(&msg->state);
    pthread_mutex_unlock(&state_mutex);}
// ----

/*
    Receives game updates
*/
void receive(){ // Receive a message, parse it and update state
    message msg;
    char command[16];

    receive_message(serv_socket, &msg);
    sscanf(msg.txt, "%s", command);

    if(strcmp(command, "ball_info") == 0){
        parse_ball_info(&msg);
    }else if(strcmp(command, "field_status") == 0){
        parse_field_status(&msg);
    }else if(strcmp(command, "dead") == 0){
        parse_dead(&msg);
    }else if(strcmp(command, "quit") == 0){
        exit_routine();
    }else if (strcmp(command, "full") == 0) {
        serv_full = true;
    }
    free_players(&msg);
}

/*
    Sends messages to the server
*/
void send_msg(char *msg_txt){
    char command[16];
    message msg;
    msg.state.players = NULL;
    pthread_mutex_lock(&state_mutex);
    create_message(&msg, msg_txt, &game_state);
    pthread_mutex_unlock(&state_mutex);
    sscanf(msg.txt, "%s", command);

    if(strcmp(command, "connect") == 0){
        send_message(serv_socket, &msg);

    }else if(strcmp(command, "move") == 0){
        send_message(serv_socket, &msg);
    
    }else if(strcmp(command, "continue") ==0){
        send_message(serv_socket, &msg);
    }
    free_players(&msg);
}

void* receive_and_render(void* arg){

    while(1){
        receive();
        if(player.health <= 0){
            dead=true;
        }
        pthread_mutex_lock(&state_mutex);
        if(!dead)render(game_screen, &game_state);
        else continue_window(&game_screen);
        pthread_mutex_unlock(&state_mutex);
    }
    exit_routine();
    return NULL;
}

/*
    Loop to send keys to server and get game updates
*/
void client_loop(){
    char msg[100];
    pthread_t recv_render;

    send_msg("connect");
    receive();
    if (serv_full) return;

    pthread_create(&recv_render, NULL, receive_and_render, NULL);

    int key=-1;
    while(key != 'q' && key != 27){
        key = getch();
        if(dead){
            send_msg("continue");
            player.health=MAX_HP;
            dead=false;
        }

        dir direction = key_to_dir(key);

        if (direction != DIR_NONE) {
            sprintf(msg, "move %d", direction);
            send_msg(msg);
        }

    }
    pthread_cancel(recv_render);
    pthread_join(recv_render, NULL);
    exit_routine();

}

void initGame(){
    game_state.num_bots=0;
    game_state.num_players=0;
    game_state.num_prizes=0;
    game_state.players = NULL;
}

int main(int argc, char **argv ){
    char address[108];
    int port;
    if (argc != 3) {
        printf("Usage: ./chase-client <server_address> <server_port>\n");
        exit(0);
    }

    init_window(&game_screen);
    cbreak();				/* Line buffering disabled	*/

    strcpy(address,argv[1]);
    port = atoi(argv[2]);
    serv_socket = tcp_connect(address, port);

    initGame();
    client_loop();
    exit(0);
}
