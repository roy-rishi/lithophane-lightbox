#include "led_panel_utils.h"
#include "led_strip.h"
#include "led_panel_config.h"
#include "gamma.h"

void set_pixel(led_strip_handle_t p, uint8_t i, uint8_t r,uint8_t g,uint8_t b) {
    led_strip_set_pixel(p, i, gamma_table[r], gamma_table[g], gamma_table[b]);
}

void set_pixel_hsv(led_strip_handle_t p, uint8_t i, uint8_t h,uint8_t s,uint8_t v) {
    led_strip_set_pixel_hsv(p, i, h, s, gamma_table[v]);
}

void fill_all(led_strip_handle_t p, uint8_t r, uint8_t g, uint8_t b) {
    for (int i = 0; i < NUM_LEDS; i++)
        led_strip_set_pixel(p, i, gamma_table[r], gamma_table[g], gamma_table[b]);
}
