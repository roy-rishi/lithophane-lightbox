idf_component_register(SRCS "ble.c" "gap.c" "gatt.c"
                       INCLUDE_DIRS "."
                       REQUIRES nvs_flash bt)