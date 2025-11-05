idf_component_register(SRCS "navigation_control.c" "utils.c" "feature_detection.c" "motor_control.c" "QTRX-MD-NRC.c" "main.c" "esp_now_comms.c" "QTRX-MD-NRC.c" "feature_detection.c" "utils.c" "navigation_control.c"
                    PRIV_REQUIRES esp_wifi nvs_flash esp_driver_gpio esp_timer esp_driver_ledc esp_driver_pcnt
                    INCLUDE_DIRS "../include")