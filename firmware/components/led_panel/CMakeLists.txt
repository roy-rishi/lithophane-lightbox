idf_component_register(SRCS "led_panel.c" "led_panel_utils.c" "gamma.c" "led_panel_pulse.c"
                       INCLUDE_DIRS "."
                       REQUIRES led_strip)