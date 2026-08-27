#include "led.h"

#include <freertos/FreeRTOS.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "status_codes.h"

#define TAG "LED"
#define STATUS_LED_PIN 2

extern QueueHandle_t led_queue;

void led_init() {
    ESP_LOGI(TAG, "Initializing LED on pin %d...", STATUS_LED_PIN);
    gpio_reset_pin(STATUS_LED_PIN);
    gpio_set_direction(STATUS_LED_PIN, GPIO_MODE_OUTPUT);
}

void led_on() {
    gpio_set_level(STATUS_LED_PIN, 1);
}

void led_off() {
    gpio_set_level(STATUS_LED_PIN, 0);
}

static void status_task(void* param) {
    ESP_LOGI(TAG, "Status LED task started");

    Status status = ERROR;
    while (1) {
        if (status == CONNECTED) {
            // solid
            led_on();
            xQueueReceive(led_queue, &status, portMAX_DELAY);
        }
        if (status == ADVERTISING) {
            // flash twice
            led_on();
            vTaskDelay(50 / portTICK_PERIOD_MS);
            led_off();
            vTaskDelay(150 / portTICK_PERIOD_MS);
            led_on();
            vTaskDelay(50 / portTICK_PERIOD_MS);
            led_off();
            xQueueReceive(led_queue, &status, 2000 / portTICK_PERIOD_MS);
        }
        if (status == ERROR) {
            // flash rapidly
            led_on();
            vTaskDelay(100 / portTICK_PERIOD_MS);
            led_off();
            xQueueReceive(led_queue, &status, 100 / portTICK_PERIOD_MS);
        }
    }

    vTaskDelete(NULL);
}

void status_led_start() {
    xTaskCreate(status_task, "Status", 4 * 1024, NULL, 5, NULL);
}
