file(GLOB_RECURSE srcs "main.c" "./*.c")

idf_component_register(SRCS "${srcs}"
                        PRIV_REQUIRES esp_driver_gpio nvs_flash bt
                        INCLUDE_DIRS ".")