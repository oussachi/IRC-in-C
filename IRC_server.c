#include<stdio.h>
#include<string.h>
#include"socket_setup.h"
#include"IRC_structs.h"

#define CRLF "\r\n"
#define MAX_MSG_LEN 512
#define PORT 2525
#define NUM_REQUESTS 1

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
    char *token;

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
    strcpy(irc_message->trailing, strchr(input, ':')); // copy the trailing
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


//int find_client_by_nickname(char *nickname, client *client_list) {
//    client current_client;
//    for(int i = 0; i < sizeof(client_list); i++) {
//        current_client = client_list[i];
//        if(strcmp(current_client.nickname, nickname) == 0) {
//            return 0;
//        }
//    }
//    return 1;
//}

int handle_nick(client *c, char *nickname) {
    strcpy(c->nickname, nickname);
    if(strcmp(c->username, "") != 0) {
        c->registered = 1;
    } 
    return 0;
}


int main() {
    int sock_fd, client_sock_fd;
    char data[512];
    irc_message irc_message;
    server_state server_state;
    client client;

    sock_fd = server_start(PORT, NUM_REQUESTS);
    client_sock_fd = server_accept(sock_fd);
    client_init(&client, client_sock_fd);
    while(1) {
        socket_recv_data(client_sock_fd, data);
        parse_input(data, &irc_message);
        if(strcmp(irc_message.command, "NICK") == 0) handle_nick(&client, irc_message.params[0]);
        printf("Client nickname : %s\n", client.nickname);
    }

}