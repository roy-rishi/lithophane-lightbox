#include "led.h"

#include "config.h"
#include "driver/gpio.h"

void led_init() {
    gpio_reset_pin(STATUS_LED_PIN);
    gpio_set_direction(STATUS_LED_PIN, GPIO_MODE_OUTPUT);
}

void led_on() {
    gpio_set_level(STATUS_LED_PIN, 1);
}

void led_off() {
    gpio_set_level(STATUS_LED_PIN, 0);
}
