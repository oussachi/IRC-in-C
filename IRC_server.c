#include<stdio.h>
#include<string.h>
#include"socket_setup.h"
#include"IRC_structs.h"

#define MAX_MSG_LEN 512
#define PORT 2525
#define NUM_REQUESTS 1


/* --------------------------- Channel functions --------------------------- */
// Function to initialize the channel structs, to avoid searching/writing in arbitrary addresses
int channel_init(channel *c) {
    strcpy(c->name, "");
    c->member_num = 0;
    c->members = malloc(sizeof(client));
    return 0;
}

// Function to search for a channel in a server state
int find_channel_by_name(char *channel_name, server_state *sc) {
    for(int i = 0; i < MAX_CHANNELS; i++) {
        if(strcmp(sc->channels[i]->name, channel_name) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to create a channel and add it to the server's state
int add_channel_to_server(server_state *sc, channel *c) {
    for(int i = 0; i < MAX_CHANNELS; i++) {
        if(!strcmp(sc->channels[i]->name, "")) {
            sc->channels[i] = c;
            return 0;
        }
    }
    return 1;
}


// A useful function to see all server's channels for debugging
int server_channels(server_state *sc) {
    for(int i = 0; i < MAX_CHANNELS; i++) {
        printf("Channel %d : %s\n", i, sc->channels[i]->name);
        for(int j = 0; j < sc->channels[i]->member_num; j++) {
            printf("\tMember %d : %s\n", j, sc->channels[i]->members[i].nickname);
        }
    }

    return 0;
}
/* ------------------------------------------------------------------------- */


/* --------------------------- Client functions --------------------------- */
// Function to initialize the client structs, to avoid searching/writing in arbitrary addresses
int client_init(client *c, int sock_fd) {
    c->socket_fd = sock_fd;
    strcpy(c->nickname, "");
    strcpy(c->username,"");
    strcpy(c->realname,"");
    c->registered = 0;
    c->channel_num = 0;
    c->channels = malloc(sizeof(char *));
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


// A useful function to see all server's clients for debugging
int server_clients(server_state *sc) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        printf("Client %d : %s, %s, %s, registered[%d]\n", i, sc->clients[i]->nickname, sc->clients[i]->username, sc->clients[i]->realname, sc->clients[i]->registered);
        for(int j = 0; j < sc->clients[i]->channel_num; j++) {
            printf("\tChannel : %s\n", sc->clients[i]->channels[j]);
        }
    }

    return 0;
}
/* ------------------------------------------------------------------------ */



/* --------------------------- Server functions -------------------------- */
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

// Function to init the server
int server_init(server_state *sc) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        sc->clients[i] = malloc(sizeof(client));
        client_init(sc->clients[i], -1);
    }
    for(int i = 0; i < MAX_CHANNELS; i++) {
        sc->channels[i] = malloc(sizeof(channel));
        channel_init(sc->channels[i]);
    }
    return 0;
}

int server_close(server_state *sc) {
    for(int i = 0; i < MAX_CLIENTS; i++) {
        free(sc->clients[i]);
    }
    for(int i = 0; i < MAX_CHANNELS; i++) {
        free(sc->channels[i]);
    }
    return 0;
}
/* ----------------------------------------------------------------------- */



/* --------------------------- Commands handling --------------------------- */
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



// Function to join / create a channel
// Format : JOIN #<channel_name>
int handle_join(channel *c, char *channel_name, server_state *sc, client *cl) {
    if(find_channel_by_name(channel_name, sc)) {
        return 0;
    }
    else {
        strcpy(c->name, channel_name);
        add_channel_to_server(sc, c);

        // Add the client to the channel's member list
        c->members = realloc(c->members, sizeof(client) * (c->member_num + 1));
        c->members[c->member_num] = *cl;
        c->member_num++;

        // Add the channel to the client's channel list
        cl->channels = realloc(cl->channels, sizeof(char *) * (cl->channel_num + 1));
        cl->channels[cl->channel_num] = malloc(15);
        strcpy(cl->channels[cl->channel_num], channel_name);
        cl->channel_num++;
        return 0;
    }
}
/* ------------------------------------------------------------------------- */



int main() {
    int sock_fd, client_sock_fd;
    char *data = malloc(512);
    irc_message irc_message;
    server_state server_state;
    client client;
    channel channel;

    sock_fd = server_start(PORT, NUM_REQUESTS);
    client_sock_fd = server_accept(sock_fd);
    client_init(&client, client_sock_fd);
    channel_init(&channel);
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
                continue;
            }
        }
        if(strcmp(irc_message.command, "JOIN") == 0) {
            if(irc_message.param_num > 1) {
                socket_send_data(client_sock_fd, "Too many arguments for JOIN\n");
            }
            else if(irc_message.params[0][0] != '#') {
                socket_send_data(client_sock_fd, "Channel name should start with #\n");
            }
            else {
                handle_join(&channel, irc_message.params[0], &server_state, &client);
            }
        }

        server_channels(&server_state);
        server_clients(&server_state);
    }

    free(data);
    server_close(&server_state);
}