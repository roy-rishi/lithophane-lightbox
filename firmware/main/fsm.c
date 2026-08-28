#include "fsm.h"

#include <freertos/FreeRTOS.h>

#include "button.h"
#include "esp_log.h"
#include "led_strip_types.h"
#include "status_codes.h"

#define TAG "FSM"

extern QueueHandle_t cmd_q;
extern QueueHandle_t btn_q;
extern led_strip_handle_t panel;

State get_next_state(State cs) {
    Cmd cmd_req = {0};
    ButtonPress button_req = {0};

    // don't change states unless an event occurs
    State ns = cs;

    // check for button press
    if (xQueueReceive(btn_q, &button_req, 0)) {
        ESP_LOGI(TAG, "Handling btn press: %d", button_req);

        switch (button_req) {
            case PRESS_SHORT:
                // turn ON if idling, otherwise idle
                if (cs == S_IDLE)
                    ns = S_ON_SOLID;
                else
                    ns = S_IDLE;
                break;

            case PRESS_MEDIUM:
                // TODO: state changes
                break;

            case PRESS_LONG:
                // toggle pairing mode
                if (cs == S_PAIRING)
                    ns = S_IDLE;
                else
                    ns = S_PAIRING;
                break;

            default:
                ESP_LOGW(TAG, "Unhandled type");
        }

        return ns;
    }

    // check for BLE command request
    if (xQueueReceive(cmd_q, &cmd_req, 0)) {
        ESP_LOGI(TAG, "Handling cmd req: %d", cmd_req);

        switch (cmd_req) {
            case CMD_ON:
                ns = S_ON_SOLID;
                break;

            case CMD_OFF:
                ns = S_IDLE;
                break;

            default:
                ESP_LOGW(TAG, "Unhandled type");
        }

        return ns;
    }

    return ns;
}
