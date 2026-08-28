#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>

#include "ble.h"
#include "button.h"
#include "esp_log.h"
#include "fsm.h"
#include "led.h"
#include "led_panel.h"
#include "led_panel_pulse.h"
#include "led_panel_utils.h"
#include "led_strip.h"
#include "led_strip_types.h"
#include "status_codes.h"

#define TAG "MAIN"

// message queues
QueueHandle_t ble_status_q;
QueueHandle_t cmd_q;
QueueHandle_t button_q;
// addressable LED driver handle
led_strip_handle_t panel;

void app_main(void) {
    // initialize status LED
    led_init();

    // initialize flash storage
    ESP_LOGI(TAG, "Initializing NVS flash...");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // erase on error and try again
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
        ESP_LOGW(TAG, "Erased flash");
    }

    // create message queues
    ble_status_q = xQueueCreate(5, sizeof(uint8_t));
    cmd_q = xQueueCreate(5, sizeof(uint8_t));
    button_q = xQueueCreate(5, sizeof(uint8_t));
    if (ble_status_q == NULL || cmd_q == NULL || button_q == NULL) {
        ESP_LOGE(TAG, "Failed to create queue(s)");
        return;
    }

    // initialize and start BLE
    ble_init();
    ble_start();

    // start status LED task
    status_led_start();

    // initialize addressable LED driver
    led_panel_init(&panel);
    led_strip_clear(panel);
    led_strip_refresh(panel);

    // initialize button interrupts
    button_init();

    State cur_state = S_IDLE;
    while (1) {
        vTaskDelay(50 / portTICK_PERIOD_MS);

        // determine next state
        State next_state = get_next_state(cur_state);

        // handle state transition
        if (next_state != cur_state) {
            // TODO: actually handle BLE pairing on/off with accept list
            ESP_LOGI(TAG, "Switching to state: %d", next_state);

            if (cur_state == S_PAIRING)
                led_panel_pulse_stop();

            switch (next_state) {
                case S_IDLE:
                    led_strip_clear(panel);
                    led_strip_refresh(panel);
                    break;

                case S_ON_SOLID:
                    // TODO: read color from NVS flash
                    for (int i = 20; i < 256; i++) {
                        fill_all(panel, i, i, i);
                        led_strip_refresh(panel);
                        // wait (1960 ms total fade time)
                        vTaskDelay(10 / portTICK_PERIOD_MS);
                    }
                    break;

                case S_PAIRING:
                    led_panel_pulse_start();

                default:
                    break;
            }

            cur_state = next_state;
        }
    }
}
