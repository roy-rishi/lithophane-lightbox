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

#define TAG "MAIN"

QueueHandle_t led_queue;
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

    // create status LED task message queue
    led_queue = xQueueCreate(5, sizeof(uint8_t));
    if (led_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create queue");
        return;
    }

    // initialize and start BLE
    ble_init();
    ble_start();

    // start status LED task
    status_led_start();

    // initialize addressable LED drivers
    led_panel_init(&panel);

    while (1) {
        // TODO: remove gamma test
        led_strip_clear(panel);
        for (int i = 0; i <= 255; i++) {
            led_strip_set_pixel(panel, 0, i, i, i);
            set_pixel(panel, 2, i, i, i);
            set_pixel_hsv(panel, 4, 0, 0, i);

            led_strip_refresh(panel);
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        for (int i = 255; i >= 0; i--) {
            led_strip_set_pixel(panel, 0, i, i, i);
            set_pixel(panel, 2, i, i, i);
            set_pixel_hsv(panel, 4, 0, 0, i);

            led_strip_refresh(panel);
            vTaskDelay(20 / portTICK_PERIOD_MS);
        }
        led_strip_clear(panel);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
