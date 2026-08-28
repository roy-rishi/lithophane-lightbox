#ifndef LED_PANEL_PULSE_
#define LED_PANEL_PULSE_

#include "led_strip_types.h"

// Start a task to pulse the panel
void led_panel_pulse_start();
// Stop the task that pulses the panel. Idempotent.
void led_panel_pulse_stop();

#endif  // LED_PANEL_PULSE_
