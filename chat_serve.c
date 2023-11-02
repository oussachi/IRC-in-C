#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_setup.h"

#define PORT 7890 // the port users connect to

int main(void)
{
    struct sockaddr_in client_addr;
    socklen_t sin_size;
    int sockfd, new_sockfd;
    int recv_length, input_counter;
    char buffer[1024], send_buffer[1024];
    char input_char;

    sockfd = socket_listen(PORT, 5);
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