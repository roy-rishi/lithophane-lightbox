#ifndef STATUS_CODES_
#define STATUS_CODES_

typedef enum status_ {
    ADVERTISING,
    CONNECTED,
    ERROR
} Status;

typedef enum cmd_ {
    CMD_ON,
    CMD_OFF,
    CMD_COLOR_CHG
} Cmd;

#endif  // STATUS_CODES_
