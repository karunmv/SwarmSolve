#include "main.h"
#include "esp_now_comms.h"
#include "QTRX-MD-NRC.h"




void app_main(void) {
    uint32_t ir_pins[IR_PIN_COUNT] = {IR_PIN_1, IR_PIN_2, IR_PIN_3, IR_PIN_4, IR_PIN_5, IR_PIN_6, IR_PIN_7, IR_PIN_8};
    uint32_t ir_values[IR_PIN_COUNT];

    // Initialize communications
    wifi_sta_init();
    esp_broadcast_setup();

    
    int state = 1;
    while(1) {

        // int ir[1] = {5};
        read_ir_sensor_array(&(ir_pins[0]), &(ir_values[0]), IR_PIN_COUNT);

        printf("IR_ARRAY Values: [ ");
        for (int i = 0; i < IR_PIN_COUNT; i++) {
            printf("%ld ", ir_values[i]);
        }
        printf("]\n");

        // send_message();


        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
