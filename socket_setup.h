#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hacking.h"

int socket_listen(int PORT, int NUM_REQUESTS)
{
    int sockfd, new_sockfd; // Listen on sock_fd, new connection on new_fd
    struct sockaddr_in host_addr, client_addr;
    // My address information
    int yes = 1;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        fatal("in socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) // setting socket option SO_REUSEADDR to true
        fatal("setting socket option SO_REUSEADDR");
    host_addr.sin_family = AF_INET; // defining the address family
    // Host byte order
    host_addr.sin_port = htons(PORT); // Short, network byte order
    host_addr.sin_addr.s_addr = 0;    // Automatically fill with my IP.
    memset(&(host_addr.sin_zero), '\0', 8);
    if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1) // binding the socketfd to the IPv4 host socket we made
        fatal("binding to socket");
    if (listen(sockfd, NUM_REQUESTS) == -1) // listening to requests sent to host_addr, up to a maximum of 5
        fatal("listening on socket");
    return sockfd;
}