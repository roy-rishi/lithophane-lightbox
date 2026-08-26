#include "led.h"

#include "driver/gpio.h"
#include "esp_log.h"

#define TAG "LED"
#define STATUS_LED_PIN 2

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
