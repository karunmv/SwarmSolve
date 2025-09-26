#include "main.h"
#include "esp_now_comms.h"


void app_main(void) {
    // Initialize communications
    wifi_sta_init();
    esp_broadcast_setup();

  
    while(1) {
        send_message();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
