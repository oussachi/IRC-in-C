#define MAX_CLIENTS 10
#define MAX_CHANNELS 5
#define MAX_MEMBERS 10

typedef struct {
    int socket_fd;
    char nickname[15];
    char username[15];
    char realname[15];
    int registered;
    int channel_num;
    char **channels;
} client;

typedef struct {
    int member_num;
    char name[15];
    client *members;
} channel;

typedef struct {
    client *clients[MAX_CLIENTS];
    channel *channels[MAX_CHANNELS];
} server_state;

typedef struct {
    char command[10];
    char params[15][64];
    int param_num;
    char trailing[512];
} irc_message;