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

    while(1) {
        client_sock_fd = socket_accept(sock_fd, "NEW CONNECTION\r\n");
        if(client_sock_fd < 0) continue;
        socket_recv_data(client_sock_fd, "DATA RECEIVED\r\n");
    }
}

int main() {
    int sock_fd;

    sock_fd = server_start(PORT, NUM_REQUESTS);
    server_accept(sock_fd);
}