idf_component_register(SRCS "motor_control.c" "QTRX-MD-NRC.c" "main.c" "esp_now_comms.c" "QTRX-MD-NRC.c"
                    PRIV_REQUIRES esp_wifi nvs_flash esp_driver_gpio esp_timer esp_driver_ledc
                    INCLUDE_DIRS "../include")