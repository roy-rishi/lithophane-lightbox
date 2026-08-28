#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "led_strip_types.h"
#include "led_panel_pulse.h"
#include "led_panel_utils.h"

static TaskHandle_t task;
extern led_strip_handle_t panel;

static void pulse_task(void *arg) {
    while (1) {
        for (int i = 15; i < 256; i++) {
            fill_all(panel, 0, 0, i);
            led_strip_refresh(panel);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
        for (int i = 255; i >= 15; i--) {
            fill_all(panel, 0, 0, i);
            led_strip_refresh(panel);
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
    vTaskDelete(NULL);
}

void led_panel_pulse_start() {
    xTaskCreate(pulse_task, "Pulse", 4 * 1024, NULL, 5, &task);
}

void led_panel_pulse_stop() {
    // prevent double deletion (ensure idempotency)
    if (task == NULL)
        return;

    vTaskDelete(task);
    task = NULL;
}