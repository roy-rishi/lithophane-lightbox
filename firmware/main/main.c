#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <nvs_flash.h>
#include <stdio.h>

#include "config.h"
#include "esp_log.h"
#include "host/ble_hs.h"
#include "host/ble_uuid.h"
#include "host/util/util.h"
#include "led.h"
#include "nimble/ble.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"

void app_main(void) {
    // initialize status LED
    ESP_LOGI(TAG, "Initializing LED on pin %d", STATUS_LED_PIN);
    led_init();

    // initialize NVS flash storage, erasing it on error and trying again
    ESP_LOGI(TAG, "Initializing NVS flash");
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "Erasing flash");
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    // initialize NimBLE stack
    ESP_LOGI(TAG, "Initializing NimBLE");
    ESP_ERROR_CHECK(nimble_port_init());

    // initialize GAP service for advertising
    ble_svc_gap_init();
    ble_svc_gap_device_name_set(GAP_NAME);

    // TODO...

    while (1) {
        led_on();
        vTaskDelay(500 / portTICK_PERIOD_MS);
        led_off();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
