/* Status LED operations */
#ifndef LED_H_
#define LED_H_

// Initialize the status LED GPIO pin
void led_init();
// Turn on the status LED
void led_on();
// Turn off the status LED
void led_off();
// Start a task for flashing the status pattern
void status_led_start();

#endif  // LED_H_
