#include "main.h"
#include "esp_now_comms.h"
#include "QTRX-MD-NRC.h"
#include "motor_control.h"
#include "feature_detection.h"
#include "utils.h"


void app_main(void) {

    /* PIN ARRAYS */
    uint32_t ir_pins[IR_PIN_COUNT] = {IR_PIN_1, IR_PIN_2, IR_PIN_3, IR_PIN_4, IR_PIN_5, IR_PIN_6, IR_PIN_7, IR_PIN_8};
    uint32_t motor_enable_pins[NUM_MOTORS] = {M1_PWM, M2_PWM};
    uint32_t motor_phase_pins[NUM_MOTORS] = {M1_DIR, M2_DIR};
    
    /* Variable declarations */
    uint32_t ir_values[IR_PIN_COUNT];
    uint8_t feature_state;


    /* Communication Initialization */
    // wifi_sta_init();
    // esp_broadcast_setup();

    /* Motor Pin Initialization */
    motor_setup(motor_enable_pins, motor_phase_pins);


    /* IR Sensor Testing */
    while(1) {


        read_ir_sensor_array(&(ir_pins[0]), &(ir_values[0]), IR_PIN_COUNT);

        update_feature_state(&feature_state, ir_values);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}