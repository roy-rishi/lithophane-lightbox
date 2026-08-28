#include "button.h"

#include <freertos/FreeRTOS.h>

#include "driver/gpio.h"
#include "esp_timer.h"
#include "status_codes.h"

#define BUTTON_PIN 15

extern QueueHandle_t button_q;

static volatile int64_t last_time = 0;

static void IRAM_ATTR button_isr(void* arg) {
    // get current time and button level
    int64_t now = esp_timer_get_time();
    int button_lvl = gpio_get_level(BUTTON_PIN);

    if (button_lvl == 0) {
        // button pressed, save time
        last_time = now;
    } else {
        // button released, check time held
        int64_t delta = now - last_time;

        // pressed for 4000 ms
        if (delta > 4000000LL) {
            ButtonPress data = PRESS_LONG;
            xQueueSendFromISR(button_q, &data, 0);
            return;
        }

        // pressed for 1000 ms
        if (delta > 1000000LL) {
            ButtonPress data = PRESS_MEDIUM;
            xQueueSendFromISR(button_q, &data, 0);
            return;
        }

        // pressed for 20 ms
        if (delta > 20000LL) {
            ButtonPress data = PRESS_SHORT;
            xQueueSendFromISR(button_q, &data, 0);
            return;
        }
    }
}

void button_init() {
    // configure interrupt
    gpio_config_t config = {
        .intr_type = GPIO_INTR_ANYEDGE,
        .mode = GPIO_MODE_INPUT,
        .pin_bit_mask = (1ULL << BUTTON_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_ENABLE, // TODO: disable if HW duplicates this
    };
    gpio_config(&config);

    // initialize and attach ISR
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BUTTON_PIN, button_isr, NULL);
}
