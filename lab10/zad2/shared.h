#define NAME_LEN 16
#define MAX_CLIENTS 16

struct game_state {
    char board[9];
    char move; 
    int num_fields;
};

typedef struct message {
enum type { 
    connected, disconnected, start,
    end, wait, moved,
    state, ping, username_taken, server_full
} type;
struct content {
    char name[NAME_LEN];
    char symbol;
    int move;
    struct game_state state;
} content;
} message;