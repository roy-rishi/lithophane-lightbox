idf_component_register(SRCS "main.c"
                        REQUIRES nvs_flash bt bluetooth status_led common
                        INCLUDE_DIRS ".")