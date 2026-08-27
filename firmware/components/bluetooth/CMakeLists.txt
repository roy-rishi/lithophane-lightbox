idf_component_register(SRCS "ble.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash bt common)