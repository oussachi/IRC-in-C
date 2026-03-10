typedef struct {
    int socket_fd;
    char* nickname;
    char* username;
    char* realname;
    bool registered;
    char* channels[];
} client;

typedef struct {
    char* name;
    client members[];
} channel;

typedef struct {
    client clients[];
    channel channels[];
} server_state;