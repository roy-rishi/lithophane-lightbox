#ifndef LED_PANEL_UTILS_
#define LED_PANEL_UTILS_

#include "gamma.h"
#include "led_strip_types.h"
#include "stdint.h"
#include "led_strip.h"

// Set pixel values with gamma-correction
void set_pixel(led_strip_handle_t p, uint8_t i, uint8_t r,uint8_t g,uint8_t b);
void set_pixel_hsv(led_strip_handle_t p, uint8_t i, uint8_t h,uint8_t s,uint8_t v);

// Fill all LEDs with gamma-correction
void fill_all(led_strip_handle_t p, uint8_t r, uint8_t g, uint8_t b);

#endif  // LED_PANEL_UTILS_
