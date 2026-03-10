CC := gcc

IRC_server: IRC_server.c  socket_setup.h
	gcc IRC_server.c -o IRC_server -Wall -Wextra -pedantic -std=c99

clean:
	rm IRC_server