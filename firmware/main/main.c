#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>

#include "ble.h"
#include "esp_log.h"
#include "led.h"
#include "led_panel.h"

#define TAG "MAIN"

QueueHandle_t led_queue;

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
    led_panel_init();

    while (1) {
        vTaskDelay(1000);
    }
}
