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
        strcpy(irc_message->params[i], token);
        token = strtok(NULL, " ");
        i++;
    }
    strcpy(irc_message->trailing, strchr(input, ':'));
    irc_message->param_num = i;

    free(input_copy);
}

int main() {
    int sock_fd, client_sock_fd;
    char data[512];
    irc_message irc_message;

    sock_fd = server_start(PORT, NUM_REQUESTS);
    client_sock_fd = server_accept(sock_fd);
    while(1) {
        socket_recv_data(client_sock_fd, "DATA RECEIVED\r\n", data);
        parse_input(data, &irc_message);
        printf("Command %s\n", irc_message.command);
    }

}