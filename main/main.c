#include "main.h"
#include "esp_now_comms.h"
#include "QTRX-MD-NRC.h"
#include "motor_control.h"


void app_main(void) {
    uint32_t ir_pins[IR_PIN_COUNT] = {IR_PIN_1, IR_PIN_2, IR_PIN_3, IR_PIN_4, IR_PIN_5, IR_PIN_6, IR_PIN_7, IR_PIN_8};
    uint32_t ir_values[IR_PIN_COUNT];
    uint32_t motor_enable_pins[2] = {M1_PWM, M2_PWM};
    uint32_t motor_phase_pins[2] = {M1_DIR, M2_DIR};

    // Initialize communications
    // wifi_sta_init();
    // esp_broadcast_setup();

    /* Motor Testing */
    motor_setup(motor_enable_pins, 2, motor_phase_pins, 2);
    while (1) {
        
        for (int i = 0; i < 100; i++) {
            robot_state(motor_phase_pins, 2, i, FORWARD);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }

    /* IR Sensor Testing */
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