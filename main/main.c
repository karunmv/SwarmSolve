#include "main.h"
#include "esp_now_comms.h"

#include <stdio.h>

void app_main(void) {
    
    /* Communication Initialization */
    wifi_sta_init();
    esp_broadcast_setup();


    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}