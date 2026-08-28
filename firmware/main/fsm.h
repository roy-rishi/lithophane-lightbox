#ifndef FSM_
#define FSM_

// Operating states. Mostly independent of BLE status (apart from pairing mode).
typedef enum states_ {
    S_IDLE,  // panel OFF
    S_ON_SOLID,  // solid color
    S_PAIRING,  // BLE pairing mode. Panel pulses blue.
} State;

State get_next_state(State cs);

#endif  // FSM_
