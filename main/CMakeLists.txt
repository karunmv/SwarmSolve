idf_component_register(SRCS "main.c" "esp_now_comms.c"
                    PRIV_REQUIRES esp_wifi nvs_flash
                    INCLUDE_DIRS "../include")