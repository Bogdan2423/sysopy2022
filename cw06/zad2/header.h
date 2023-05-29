#define MAX_SIZE 256
#define QUEUE_NAME "/chat_queue"

typedef enum com_type {
    LIST = 2, 
    ALL = 3,
    ONE = 4,
    STOP = 1,
    INIT = 5
} com_type;
