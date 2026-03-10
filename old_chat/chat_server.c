#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "socket_setup.h"

#define PORT 7890 // the port users connect to
#define NUM_REQUESTS 5 // max number of requests

int main(void)
{
    int sockfd;

    sockfd = socket_listen(PORT, 5);
    while (1)
    {
        socket_accept(sockfd);
    }
    return 0;
}