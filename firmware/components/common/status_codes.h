#ifndef STATUS_CODES_
#define STATUS_CODES_

typedef enum ble_status_ {
    BLE_ADVERTISING,
    BLE_CONNECTED,
    BLE_ERROR
} Status;

typedef enum cmd_ {
    CMD_ON,
    CMD_OFF,
    CMD_SET_COLOR
} Cmd;

#endif  // STATUS_CODES_
