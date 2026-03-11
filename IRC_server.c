#include<stdio.h>
#include<string.h>
#include"socket_setup.h"
#include"IRC_structs.h"

#define MAX_MSG_LEN 512
#define PORT 2525
#define NUM_REQUESTS 1
#define MAX_CLIENTS 100
#define MAX_CHANNELS 50


int server_start(int port, int num_requests) {
    int sock_fd = socket_listen(port, num_requests);
    if(sock_fd < 0) {
        perror("Server init\n");
        return 1;
    }
    else printf("%s", "Server initiated\n");
    return sock_fd;
}

int server_accept(int sock_fd) {
    int client_sock_fd;
    client_sock_fd = socket_accept(sock_fd, "NEW CONNECTION\r\n");
    if(client_sock_fd < 0) return -1;
    return client_sock_fd;
}


void parse_input(char *input, irc_message *irc_message) {
    char *input_copy = malloc(512);
    char *token, *trailing;

    strcpy(input_copy, input);
    token = strtok(input_copy, " ");
    if(token == NULL) exit(1);
    strcpy(irc_message->command, token); // copy the command from the input to the struct'

    token = strtok(NULL, " ");
    int i = 0;
    while(token != NULL && token[0] != ':') {
        strcpy(irc_message->params[i], token); // copy the parameters
        token = strtok(NULL, " ");
        i++;
    }
    trailing = strchr(input, ':');
    if(trailing != NULL) {
        strcpy(irc_message->trailing, trailing); // copy the trailing
    } else {
        strcpy(irc_message->trailing, "\0"); // copy the trailing
    }
    irc_message->param_num = i;

    free(input_copy);
}

int client_init(client *c, int sock_fd) {
    c->socket_fd = sock_fd;
    strcpy(c->nickname, "");
    strcpy(c->username,"");
    strcpy(c->realname,"");
    c->registered = 0;
    return 0;
}

int server_init(server_state *sc) {
    sc->clients = malloc(sizeof(client) * MAX_CLIENTS);
    sc->channels = malloc(sizeof(channel) * MAX_CHANNELS);

    for(int i = 0; i < MAX_CLIENTS; i++) {
        client_init(&sc->clients[i], -1);
    }
    return 0;
}

int add_client_to_server(server_state *sc, client *c) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(sc->clients[i].socket_fd == -1) {
            sc->clients[i] = *c;
            return 0;
        }
    }
    return 1;
}

int find_client_by_nickname(char *nickname, server_state *sc) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        //printf("client's name %s vs nickname %s\n", sc->clients[i].nickname, nickname);
        if(strcmp(sc->clients[i].nickname, nickname) == 0) {
            printf("Nickname taken\n");
            return 1;
        }
    }
    return 0;
}

int handle_nick(client *c, char *nickname, server_state *sc) {
    strcpy(c->nickname, nickname);
    if(strcmp(c->username, "") != 0) {
        c->registered = 1;
    }
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(strcmp(sc->clients[i].nickname,"") == 0) {
            strcpy(sc->clients[i].nickname, nickname);
        }
    }
    return 0;
}


int main() {
    int sock_fd, client_sock_fd;
    char *data = malloc(512);
    irc_message irc_message;
    server_state server_state;
    client client;

    sock_fd = server_start(PORT, NUM_REQUESTS);
    client_sock_fd = server_accept(sock_fd);
    client_init(&client, client_sock_fd);
    server_init(&server_state);
    add_client_to_server(&server_state, &client);
    while(1) {
        socket_recv_data(client_sock_fd, data);
        parse_input(data, &irc_message);
        if((strcmp(irc_message.command, "NICK") == 0) && find_client_by_nickname(irc_message.params[0], &server_state) == 0) {
            handle_nick(&client, irc_message.params[0], &server_state);
        }
        printf("setting Client nickname : %s\n", client.nickname);
    }

    free(data);
}