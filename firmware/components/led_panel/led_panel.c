#include <freertos/FreeRTOS.h>

#include "gamma.h"
#include "led_strip.h"
#include "led_strip_rmt.h"
#include "led_strip_types.h"

#define PANEL_PIN 13
#define NUM_LEDS 5

// macros for setting pixel values with gamma correction
#define set_pixel(i, r, g, b) led_strip_set_pixel(panel, i, gamma_table[r], gamma_table[g], gamma_table[b])
#define set_pixel_hsv(i, h, s, v) led_strip_set_pixel_hsv(panel, i, h, s, gamma_table[v]);

static led_strip_handle_t panel = NULL;

void led_panel_init() {
    /// LED config
    led_strip_config_t strip_config = {
        .strip_gpio_num = PANEL_PIN,
        .max_leds = NUM_LEDS,
        .led_model = LED_MODEL_WS2812,
        .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB,  // GRB color order
        .flags = {
            .invert_out = false,  // non-inverted output signal
        },
    };

    /// RMT backend config
    led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,  // RMT counter clock frequency: 10MHz
        .mem_block_symbols = 64,            // the memory size of each RMT channel, in words (4 bytes)
        .flags = {
            .with_dma = false,  // DMA feature is available on chips like ESP32-S3/P4
        },
    };

    // initialize
    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &panel));

    // TODO: remove gamma test
    led_strip_clear(panel);
    for (int i = 0; i <= 255; i++) {
        led_strip_set_pixel(panel, 0, i, i, i);
        set_pixel(2, i, i, i);
        set_pixel_hsv(4, 0, 0, i);

        led_strip_refresh(panel);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    for (int i = 255; i >= 0; i--) {
        led_strip_set_pixel(panel, 0, i, i, i);
        set_pixel(2, i, i, i);
        set_pixel_hsv(4, 0, 0, i);

        led_strip_refresh(panel);
        vTaskDelay(20 / portTICK_PERIOD_MS);
    }
    led_strip_clear(panel);
}
