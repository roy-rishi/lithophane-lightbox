idf_component_register(SRCS "main.c" "led.c"
                        REQUIRES esp_driver_gpio nvs_flash bt bluetooth
                        INCLUDE_DIRS ".")