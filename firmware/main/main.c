#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>

#include "ble.h"
#include "esp_log.h"
#include "led.h"
#include "led_panel.h"
#include "led_strip.h"
#include "led_strip_types.h"
#include "led_panel_utils.h"
#include "status_codes.h"
#include "button.h"

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

    Cmd cmd_req = {0};
    ButtonPress button_press = {0};
    while (1) {
        // check for button press
        if (xQueueReceive(button_q, &button_press, 10)) {
            switch (button_press)
            {
            case PRESS_SHORT:
                ESP_LOGI(TAG, "Button press SHORT");
                break;

            case PRESS_MEDIUM:
                ESP_LOGI(TAG, "Button press MEDIUM");
                break;
            
            case PRESS_LONG:
                ESP_LOGI(TAG, "Button press LONG");
                break;
            
            default:
                ESP_LOGW(TAG, "Unhandled button press: %d", button_press);
            }
        }

        // check for command request
        if (xQueueReceive(cmd_q, &cmd_req, 10)) {
            switch (cmd_req) {
                // fade panel ON
                case CMD_ON:
                    for (int i = 10; i < 256; i++) {
                        fill_all(panel, i, i, i);
                        led_strip_refresh(panel);
                        // wait (1960 ms total fade time)
                        vTaskDelay(8 / portTICK_PERIOD_MS);
                    }
                    break;
    
                // turn panel OFF
                case CMD_OFF:
                    led_strip_clear(panel);
                    led_strip_refresh(panel);
                    break;
    
                default:
                    ESP_LOGW(TAG, "Unhandled cmd req: %d", cmd_req);
            }

        }
    }
}
