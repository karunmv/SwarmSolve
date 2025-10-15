#include "main.h"
#include "esp_now_comms.h"
#include "QTRX-MD-NRC.h"
#include "motor_control.h"


void app_main(void) {
    uint32_t ir_pins[IR_PIN_COUNT] = {IR_PIN_1, IR_PIN_2, IR_PIN_3, IR_PIN_4, IR_PIN_5, IR_PIN_6, IR_PIN_7, IR_PIN_8};
    uint32_t ir_values[IR_PIN_COUNT];
    uint32_t motor_pins[4] = {M1_PWM, M1_DIR, M2_PWM, M2_DIR}; 

    // Initialize communications
    wifi_sta_init();
    esp_broadcast_setup();

    motor_setup(motor_pins);
    
    // while(1){
    //     robot_state(motor_pins, FORWARD);
    //     printf("going forward\n");
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     robot_state(motor_pins, STOP);
    //     vTaskDelay(pdMS_TO_TICKS(1000));

    //     robot_state(motor_pins, REVERSE);
    //     printf("going reverse\n");
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     robot_state(motor_pins, STOP);
    //     vTaskDelay(pdMS_TO_TICKS(1000));

    //     robot_state(motor_pins, LEFT);
    //     printf("going left");
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     robot_state(motor_pins, STOP);
    //     vTaskDelay(pdMS_TO_TICKS(1000));

    //     robot_state(motor_pins, RIGHT);
    //     printf("going right");
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    //     robot_state(motor_pins, STOP);
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }

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