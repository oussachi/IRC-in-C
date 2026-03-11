typedef struct {
    int socket_fd;
    char nickname[15];
    char username[15];
    char realname[15];
    int registered;
    char **channels;
} client;

typedef struct {
    char name[15];
    client *members;
} channel;

typedef struct {
    client *clients;
    channel *channels;
} server_state;

typedef struct {
    char command[10];
    char params[15][64];
    int param_num;
    char trailing[512];
} irc_message;