#include<stdio.h>
#include<string.h>
#include"socket_setup.h"
#include"IRC_structs.h"

#define MAX_MSG_LEN 512
#define PORT 2525
#define NUM_REQUESTS 1


// function to setup and start the server
int server_start(int port, int num_requests) {
    int sock_fd = socket_listen(port, num_requests);
    if(sock_fd < 0) {
        perror("Server init\n");
        return 1;
    }
    else printf("%s", "Server initiated\n");
    return sock_fd;
}

// Function to accept connections
int server_accept(int sock_fd) {
    int client_sock_fd;
    client_sock_fd = socket_accept(sock_fd, "NEW CONNECTION\r\n");
    if(client_sock_fd < 0) return -1;
    return client_sock_fd;
}


// Function used to parse received data into IRC format :
// COMMAND param1 param2 ... :trailing
// for now we ignore the optional :prefix field in IRC messages
void parse_input(char *input, irc_message *irc_message) {
    char *input_copy = malloc(512);
    char *token, *trailing;

    strcpy(input_copy, input); // We use a copy string because strtok modifies the input string
    token = strtok(input_copy, " "); // we split by spaces
    if(token == NULL) exit(1);
    strcpy(irc_message->command, token); // copy the command from the input to the struct

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
        strcpy(irc_message->trailing, "\0"); // if no trailing, put a null byte
    }
    irc_message->param_num = i;

    free(input_copy); // freeing the copy string
}


// Function to initialize the client structs, to avoid searching/writing in arbitrary addresses
int client_init(client *c, int sock_fd) {
    c->socket_fd = sock_fd;
    strcpy(c->nickname, "");
    strcpy(c->username,"");
    strcpy(c->realname,"");
    c->registered = 0;
    return 0;
}

// Function to init the server
int server_init(server_state *sc) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        sc->clients[i] = malloc(sizeof(client));
        client_init(sc->clients[i], -1);
    }
    return 0;
}

int server_close(server_state *sc) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        free(sc->clients[i]);
    }
    return 0;
}

// Function to add a client to the server's state
int add_client_to_server(server_state *sc, client *c) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        if(sc->clients[i]->socket_fd == -1) {
            sc->clients[i] = c;
            return 0;
        }
    }
    return 1;
}

// Function to search for a nickname in a server state, used to avoid nickname collision
int find_client_by_nickname(char *nickname, server_state *sc) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        //printf("client's name %s vs nickname %s\n", sc->clients[i]->nickname, nickname);
        if(strcmp(sc->clients[i]->nickname, nickname) == 0) {
            return 1;
        }
    }
    return 0;
}


// Function to add a nickname to a user - handle the NICK command
// format : NICK <nickname>
int handle_nick(client *c, char *nickname) {
    strcpy(c->nickname, nickname);
    if(strcmp(c->username, "") != 0) {
        c->registered = 1;
        socket_send_data(c->socket_fd, "Welcome Abroad\n");
    }
    
    return 0;
}


// Function to add a username and realname - handle the USER command
// format : USER <username> <hostname> <servername> :<realname>
// The <hostname> and <servername> are historical fields set to 0 and * respectively
int handle_user(client *c, char *username, char *realname) {
    strcpy(c->username, username);
    strcpy(c->realname, realname);
    if(strcmp(c->nickname, "") != 0) {
        c->registered = 1;
        socket_send_data(c->socket_fd, "Welcome Abroad\n");
    }

    return 0;
}


// A useful function to see all server's clients for debugging
int server_clients(server_state *sc) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        printf("Client %d : %s, %s, %s, registered[%d]\n", i, sc->clients[i]->nickname, sc->clients[i]->username, sc->clients[i]->realname, sc->clients[i]->registered);
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
        if((strcmp(irc_message.command, "NICK") == 0)) {
            if(find_client_by_nickname(irc_message.params[0], &server_state))
                socket_send_data(client_sock_fd, "Nickname taken ==> Pick another one\n");
            else
                handle_nick(&client, irc_message.params[0]);
        }
        else if(strcmp(irc_message.command, "USER") == 0) {
            handle_user(&client, irc_message.params[0], irc_message.trailing);
        }
        else {
            if(client.registered == 0) {
                socket_send_data(client_sock_fd, "Not registered ==> can't perform this action\n");
            }
        }

        server_clients(&server_state);
    }

    free(data);
    server_close(&server_state);
}