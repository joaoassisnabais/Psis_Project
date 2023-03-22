#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <pthread.h>

#include "entity.h"
#include "../common/chase_internal.h"
#include "../common/chase_frontend.h"
#include "../common/defs.h"
#include "../common/message.h"
#include "../common/connection.h"

#define SOCK_PATH "/tmp/server_socket.sock"
#define INIT_PRIZES 5
#define MAX_PLAYERS 10
#define MAX_BOTS 10
#define MAX_PRIZES 10

typedef struct fd_list{
    int cfd;
    char ip[108];
    struct fd_list *next;
} fd_list;

game state;
fd_list *mail_list;
screen game_screen;
int server_socket;
pthread_mutex_t mail_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t prize_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bot_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t player_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_attr_t detached_state;

void lock_all(){
    pthread_mutex_lock(&prize_mutex);
    pthread_mutex_lock(&bot_mutex);
    pthread_mutex_lock(&player_mutex);
}

void unlock_all(){
    pthread_mutex_unlock(&prize_mutex);
    pthread_mutex_unlock(&bot_mutex);
    pthread_mutex_unlock(&player_mutex);
}

void send_all(message *msg){
    fd_list *aux=mail_list;
    while(aux!=NULL){
        send_message(aux->cfd, msg);
        aux=aux->next;
    }
}

/*
    Adds a new fd to the mail list
*/
void add_fd(fd_list *new){
    pthread_mutex_lock(&mail_mutex);
    fd_list *aux = mail_list;
    new->next = NULL;
    if(aux == NULL){
        mail_list = new;
    }else{
        while(aux->next != NULL){
            aux = aux->next;
        }
        aux->next = new;
    }
    pthread_mutex_unlock(&mail_mutex);
}

/*
    Removes fd from the mail list
*/
void remove_fd(int fd){
    pthread_mutex_lock(&mail_mutex);
    fd_list *aux = mail_list;
    fd_list *prev = NULL;
    while(aux != NULL){
        if(aux->cfd == fd){
            if(prev == NULL){
                mail_list = aux->next;
            }else{
                prev->next = aux->next;
            }
            free(aux);
            break;
        }
        prev = aux;
        aux = aux->next;
    }
    pthread_mutex_unlock(&mail_mutex);
}

void move_player (player_position_t * player, dir direction){    
    if (direction == DIR_UP){        
        if (player->y != 1){            
            player->y--;        
        }    
    }    
    if (direction == DIR_DOWN){        
        if (player->y  != WINDOW_SIZE-2){
            player->y ++;        
        }    
    }        
    if (direction == DIR_LEFT){        
        if (player->x  != 1){            
            player->x --;        
        }    
    }    
    if (direction == DIR_RIGHT) {
        if (player->x  != WINDOW_SIZE-2){            
            player->x ++;    
        } 
    }
}

void player_exit_routine(int cfd){
    close(cfd);
    remove_fd(cfd);
}

/*
    creates sends a dead message and waits to see if the player is alive after 10 seconds
*/
void* timeout_routine(void* fd){
    message msg;
    lock_all();
    create_message(&msg, "dead", &state);
    unlock_all();
    send_message(*(int*)fd, &msg);
    sleep(10);
    pthread_mutex_lock(&player_mutex);
    player_position_t *aux = getPlayerByAddr(*(int*)fd, &state);
    if(aux==NULL || aux->health <= 0){
        rmPlayerByAddr(&state, *(int*)fd);
        player_exit_routine(*(int*)fd);
        pthread_mutex_unlock(&player_mutex);
        lock_all();
        create_message(&msg, "field_status", &state);
        unlock_all();
        send_all(&msg);
    }else{
        pthread_mutex_unlock(&player_mutex);
    }
    render(game_screen, &state);
    free(fd);
    return NULL;
}

/*
    Updates positions by checking for collisions and works out collisions
*/
void updatePosition(player_position_t *player, dir direction){
    player_position_t aux = *player;
    bool is_bot, is_empty;

    move_player(&aux, direction);
    is_empty=isEmpty(aux.x, aux.y, &state);
    
    if(player->c == '*') is_bot = true;
    else is_bot = false;

    if (is_empty){    /*moves into empty space or the wall*/
        player->x = aux.x;
        player->y = aux.y;

    }else if((!is_bot) && (isPrizeCol(aux.x, aux.y, &state))){         /*moves into a health pack*/
        player->x = aux.x;
        player->y = aux.y;
        player->health += getPrizeByPos(aux.x, aux.y, &state).hp;
        rmPrizeByPos(aux.x, aux.y, &state);


    }else if(isPlayerCol(aux.x, aux.y, &state)){     /*moves into another player*/
        player_position_t *collided = getClientByPos(aux.x, aux.y, &state);
        if(!is_bot && collided->health>0) player->health++;
        if(collided->health > 0){   /*if the collided player is alive*/
            collided->health-=1;
            if(collided->health == 0){  /*if the collided player is dead*/
                int* collided_fd = (int*) malloc(sizeof(int));
                *collided_fd = getPlayerFd(&state, collided);
                pthread_t timeout_thread;
                pthread_create(&timeout_thread, &detached_state, timeout_routine, (void*) collided_fd);
            }
        }
    }

    if(!is_bot && (player->health>MAX_HP)){
        player->health=MAX_HP;
    }
}

void moveBots(){
    dir direction;
    for(int i=0; i<state.num_bots; i++){
        direction = rand() % 4;
        updatePosition(&state.bots[i], direction);
    }
}

/*
    Checks if player is alive and updates position
*/
bool updatePlayer(player_position_t *p, dir direction){
    if(p->health <= 0){ /* Healt_0 check */
        return false;
    }else{
        updatePosition(p, direction);
        return true;
    }
}

void write_ball_info(char *buffer) {
    sprintf(buffer, "ball_info %d %d %c", state.players[state.num_players-1].x, state.players[state.num_players-1].y, state.players[state.num_players-1].c);
}

/*
    Reads incoming messge and updates game variables acordingly
*/
bool parseMessage(message *msg, int client_fd){
        char command[16];
        char buffer[108];
        bool no_reply = false;
        sscanf(msg->txt, "%s", command);

        if (strcmp(command, "connect") == 0) {  /* Connect */
            pthread_mutex_lock(&player_mutex);
            addPlayer(&state, client_fd);
            pthread_mutex_unlock(&player_mutex);
            write_ball_info(buffer);
            lock_all();
            create_message(msg, buffer, &state);
            unlock_all();

        } else if (strcmp(command, "move") == 0) { /* Move */
            pthread_mutex_lock(&player_mutex);
            int n_players = state.num_players;
            pthread_mutex_unlock(&player_mutex);
            if(PlayerAuth(client_fd, n_players) == false){
                no_reply = true;
                return no_reply;
            }
            
            dir direction;
            sscanf(msg->txt, "%*s %d", (int *) &direction);   /* Get the movement key */
            lock_all();
            player_position_t *pos = getPlayerByAddr(client_fd, &state);
            if (updatePlayer(pos, direction)) {
                create_message(msg, "field_status", &state);
            }
            unlock_all();

        } else if (strcmp(command, "disconnect") == 0) { /* Disconnect */
            pthread_mutex_lock(&player_mutex);
            int n_players = state.num_players;
            pthread_mutex_unlock(&player_mutex);
            if(PlayerAuth(client_fd, n_players) == false){
                no_reply = true;
                return no_reply;
            }
            pthread_mutex_lock(&player_mutex);
            rmPlayerByAddr(&state, client_fd);
            pthread_mutex_unlock(&player_mutex);
            no_reply = true;
        
        } else if(strcmp(command, "continue") == 0) {
            pthread_mutex_lock(&player_mutex);
            player_position_t *pos = getPlayerByAddr(client_fd, &state);
            if(pos!=NULL) pos->health = MAX_HP;   /* Reset health */
            pthread_mutex_unlock(&player_mutex);
            no_reply = true;

        } else { /* Not a valid command */
            no_reply = true;
        }
        return no_reply;
}

void* player_connection(void *arg){
    fd_list *client_fd = (fd_list*)arg;
    char address[108];
    message msg;
    bool no_reply = false;
    strcpy(address, client_fd->ip);
    bool first_interaction = true;

    while (1){
        receive_message(client_fd->cfd, &msg);
        
        if(strcmp(msg.txt, "quit") == 0){
            strcpy(msg.txt, "disconnect");
            parseMessage(&msg, client_fd->cfd);
            free_players(&msg);
            break;
        }
        if(first_interaction){
            add_fd(client_fd);
            first_interaction=false;
        }
        parseMessage(&msg, client_fd->cfd);

        if (!no_reply) {            
            if(strcmp(msg.txt, "field_status") == 0)
                send_all(&msg);
            else 
                send_message(client_fd->cfd, &msg);
        }

        free_players(&msg); //message created in receive_message
        lock_all();
        render(game_screen, &state);
        unlock_all();
        no_reply = false;
    }
    lock_all();
    render(game_screen, &state);
    unlock_all();
    player_exit_routine(client_fd->cfd);
    return NULL;
}

void* spawn_prizes(void* arg){
    message msg;
    lock_all();
    initPrizes(&state, 5);
    unlock_all();
    while(1){
        sleep(5);
        pthread_mutex_lock(&prize_mutex);
        if(state.num_prizes < MAX_PRIZES){
            addPrize(&state);
            pthread_mutex_unlock(&prize_mutex);
            lock_all(); 
            render(game_screen, &state);
            create_message(&msg, "field_status", &state);
            unlock_all();
            send_all(&msg);
            free_players(&msg);   
        }else{
            pthread_mutex_unlock(&prize_mutex);
        }
    }
    return NULL;
}

void* spawn_bots(void* num_bots){
    message msg;
    pthread_mutex_lock(&bot_mutex);
    initBots(*(int*) num_bots, &state);
    pthread_mutex_unlock(&bot_mutex);
    while(1){
        sleep(3);
        pthread_mutex_lock(&bot_mutex);
        moveBots(&state);
        pthread_mutex_unlock(&bot_mutex);
        lock_all();
        render(game_screen, &state);
        create_message(&msg, "field_status", &state);
        unlock_all();
        send_all(&msg);
        free_players(&msg);
    }
    return NULL;
}

/*
    Receive calls to connect to the server 
    Check if the player is already connected
    If not, add the player to the game (create thread for the player connection)
    Send the player the current game state
*/
void serverLoop(int num_bots){
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int n_players, n_bots=num_bots;
    pthread_attr_init(&detached_state);
    pthread_attr_setdetachstate(&detached_state, PTHREAD_CREATE_DETACHED);

    pthread_t prizes_th, bots_th;
    pthread_create(&prizes_th, &detached_state, spawn_prizes, NULL);
    pthread_create(&bots_th, &detached_state, spawn_bots, (void*)&n_bots);

    while (1)
    {
        clientAddrLen = sizeof(clientAddr);
        
        fd_list* client_fd=malloc(sizeof(fd_list));
        client_fd->cfd=accept_connection(server_socket, (struct sockaddr*) &clientAddr, &clientAddrLen);
        strcpy(client_fd->ip, inet_ntoa(clientAddr.sin_addr));
        
        pthread_mutex_lock(&player_mutex);
        n_players = state.num_players;
        pthread_mutex_unlock(&player_mutex);
        
        if(PlayerAuth(client_fd->cfd, n_players) == false){
            pthread_t th;
            pthread_create(&th, &detached_state, player_connection, client_fd);
        }

    }
    close_socket(server_socket);
    pthread_mutex_destroy(&player_mutex);
    pthread_mutex_destroy(&prize_mutex);
    pthread_mutex_destroy(&bot_mutex);
    pthread_attr_destroy(&detached_state);
}

void initGame(){
    state.num_bots=0;
    state.num_players=0;
    state.num_prizes=0;
    state.players = NULL;
}


int main(int argc, char **argv){
    char address[108];
    int port, num_bots;

    if (argc != 4) {
        printf("Usage: ./chase-server <server_address> <server_port> <number_of_bots>\n");
        exit(0);
    }
    num_bots = atoi(argv[3]);
    if(num_bots > MAX_BOTS){
        printf("Number of bots must be less than %d\n", MAX_BOTS);
        exit(0);
    }

    srand(time(NULL));
    init_window(&game_screen);
    initGame();

    strcpy(address,argv[1]);
    port = atoi(argv[2]);

    server_socket= tcp_socket_init(address, port);
    serverLoop(num_bots);

    exit(0);
}
