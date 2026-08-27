idf_component_register(SRCS "main.c"
                        REQUIRES nvs_flash bt bluetooth status_led common led_panel led_strip
                        INCLUDE_DIRS ".")