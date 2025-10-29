idf_component_register(SRCS "utils.c" "esp_now_comms.c" "main.c" "utils.c"
                    PRIV_REQUIRES esp_wifi nvs_flash
                    INCLUDE_DIRS "../include")