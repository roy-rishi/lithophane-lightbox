#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>

#include "ble.h"
#include "esp_log.h"
#include "led.h"

#define TAG "MAIN"

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

    // initialize and start BLE
    ble_init();
    ble_start();

    while (1) {
        led_on();
        vTaskDelay(500 / portTICK_PERIOD_MS);
        led_off();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
