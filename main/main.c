#include "main.h"
#include "esp_now_comms.h"
#include "QTRX-MD-NRC.h"
#include "motor_control.h"
#include "feature_detection.h"
#include "navigation_control.h"
#include "utils.h"


void app_main(void) {

    /* PIN ARRAYS */
    uint32_t ir_pins[IR_PIN_COUNT] = {IR_PIN_1, IR_PIN_2, IR_PIN_3, IR_PIN_4, IR_PIN_5, IR_PIN_6, IR_PIN_7, IR_PIN_8};
    uint32_t motor_enable_pins[NUM_MOTORS] = {M1_PWM, M2_PWM};
    uint32_t motor_phase_pins[NUM_MOTORS] = {M1_DIR, M2_DIR};
    
    /* Variable declarations */
    uint32_t ir_values[IR_PIN_COUNT];
    uint8_t feature_state;
    uint8_t movement_state;
    int8_t speeds[NUM_MOTORS] = {0, 0};


    /* Communication Initialization */
    // wifi_sta_init();
    // esp_broadcast_setup();

    /* Motor Pin Initialization */
    motor_setup(motor_enable_pins, motor_phase_pins);


    /* IR Sensor Testing */
    while(1) {

        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);

        update_feature_state(&feature_state, ir_values);

        update_movement_state(feature_state, &movement_state);

        // follow_line(ir_values, motor_enable_pins, motor_phase_pins);
        if (movement_state == CHECKING_STRAIGHT) {
            check_straight(ir_pins, ir_values, motor_enable_pins, motor_phase_pins, &feature_state);
        }

        if (feature_state & STRAIGHT_CHECKED) {

            if (feature_state & RIGHT_TURN) {
                speeds[0] = 50;
                speeds[1] = -50;
                move_motors(motor_phase_pins, speeds);
                vTaskDelay(pdMS_TO_TICKS(500));
                speeds[0] = 0;
                speeds[1] = 0;
                move_motors(motor_phase_pins, speeds);
            }
        }

        if (movement_state == GOING_STRAIGHT) {
            follow_line(ir_values, motor_enable_pins, motor_phase_pins);
        } else {
            move_motors(motor_phase_pins, speeds);
        }

        print_feature_state(feature_state);
        print_movement_state(movement_state);

        print_IR_values(ir_values);

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}