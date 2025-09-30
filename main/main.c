#include "main.h"
#include "esp_now_comms.h"
#include "QTRX-MD-NRC.h"


void app_main(void) {
    // Initialize communications
    wifi_sta_init();
    esp_broadcast_setup();

    
    int state = 1;
    while(1) {

        int ir[1] = {5};
        read_ir_sensor_array(ir, 1);

        // send_message();
        // gpio_set_level(5, state);
        // state = (state + 1) % 2;


        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
