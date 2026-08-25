file(GLOB_RECURSE srcs "main.c" "./*.c")

idf_component_register(SRCS "${srcs}"
                        REQUIRES esp_driver_gpio nvs_flash bt
                        INCLUDE_DIRS ".")