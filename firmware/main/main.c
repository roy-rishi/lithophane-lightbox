#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include "config.h"
#include "led.h"

void app_main(void) {
    printf("Initializing status LED on pin %d...\n", STATUS_LED_PIN);
    led_init();

    while (1) {
        led_on();
        vTaskDelay(500 / portTICK_PERIOD_MS);
        led_off();
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
