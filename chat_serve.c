#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "hacking.h"

#define PORT 7890 // the port users connect to

int main(void)
{
    int sockfd, new_sockfd; // Listen on sock_fd, new connection on new_fd
    struct sockaddr_in host_addr, client_addr;
    // My address information
    socklen_t sin_size;
    int recv_length = 1, yes = 1, bytes_read, input_counter;
    char buffer[1024];
    char send_buffer[1024];
    char input_char;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
        fatal("in socket");
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) // setting socket option SO_REUSEADDR to true
        fatal("setting socket option SO_REUSEADDR");
    host_addr.sin_family = AF_INET; // defining the address family
    // Host byte order
    host_addr.sin_port = htons(PORT);                                               // Short, network byte order
    host_addr.sin_addr.s_addr = 0;                                                  // Automatically fill with my IP.
    memset(&(host_addr.sin_zero), '\0', 8);                                         // Zero the rest of the struct.
    if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1) // binding the socketfd to the IPv4 host socket we made
        fatal("binding to socket");
    if (listen(sockfd, 5) == -1) // listening to requests sent to host_addr, up to a maximum of 5
        fatal("listening on socket");
    while (1)
    {
        // Accept loop.
        sin_size = sizeof(struct sockaddr_in);
        new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
        if (new_sockfd == -1)
            fatal("accepting connection");
        printf("server: got connection from %s port %d\n",
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
        send(new_sockfd, "Hello, world!", 13, 0);
        send(new_sockfd, "\nSend a message > ", 18, 0);
        recv_length = recv(new_sockfd, &buffer, 1024, 0);
        while (recv_length > 0)
        {
            //dump(buffer, recv_length);
            printf("[+] : %s", buffer);
            printf("Send a message > ");
            for(input_counter = 0; (input_char = getchar()) != '\n'; input_counter++) {
                send_buffer[input_counter] = input_char;
            }
            send_buffer[input_counter] = '\n';
            send(new_sockfd, "[+] : ", 6, 0);
            send(new_sockfd, send_buffer, input_counter + 1, 0);
            send(new_sockfd, "Send a message > ", 18, 0);
            recv_length = recv(new_sockfd, &buffer, 1024, 0);
        }
        close(new_sockfd);
    }
    return 0;
}