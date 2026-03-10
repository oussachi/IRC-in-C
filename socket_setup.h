#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int socket_listen(int PORT, int NUM_REQUESTS)
{
    int sockfd; // Listen on sock_fd, new connection on new_fd
    struct sockaddr_in host_addr;
    // My address information
    int yes = 1;

    if((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
        perror("in socket");
        return -1;
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) { // setting socket option SO_REUSEADDR to true
        perror("setting socket option SO_REUSEADDR");
        return -1;
    }
    host_addr.sin_family = AF_INET; // defining the address family
    // Host byte order
    host_addr.sin_port = htons(PORT); // Short, network byte order
    host_addr.sin_addr.s_addr = 0;    // Automatically fill with my IP.
    memset(&(host_addr.sin_zero), '\0', 8);
    if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1) {// binding the socketfd to the IPv4 host socket we made
        perror("binding to socket");
        return -1;
    }
    if (listen(sockfd, NUM_REQUESTS) == -1) { // listening to requests sent to host_addr, up to a maximum of 5
        perror("listening on socket");
        return -1;
    }
    return sockfd;
}

int socket_accept(int sockfd, char* msg) {
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int new_sockfd;

    // Accept loop.
    sin_size = sizeof(struct sockaddr_in);
    new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
    if (new_sockfd == -1) {
        perror("accepting connection");
        return -1;
    }
    //printf("server: got connection from %s port %d\n",
    //       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
    printf("%s", msg);
    return new_sockfd;
}

int socket_recv_data(int client_sockfd, char* msg) {
    int recv_length;
    char buffer[1024];
    
    recv_length = recv(client_sockfd, &buffer, 1024, 0);
    while (recv_length > 0)
    {
        printf("%s", msg);
        memset(buffer, '\0', sizeof(buffer));
        recv_length = recv(client_sockfd, &buffer, 1024, 0);
    }

    close(client_sockfd);
    return 0;
}