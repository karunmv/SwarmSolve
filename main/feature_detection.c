#include "main.h"
#include "feature_detection.h"
#include "motor_control.h"
#include "QTRX-MD-NRC.h"


void update_feature_state(uint8_t *feature_state, uint32_t *ir_values) {

}

void follow_line(uint32_t *ir_values, uint32_t *motor_enable_pins, uint32_t *motor_phase_pins) {
    
    int8_t speeds[NUM_MOTORS] = {50, 50};

    if (ir_values[2] < PRIMARY_IR_THRESHOLD) {
        speeds[0] = 50;
        speeds[1] = 30;
        move_motors(motor_phase_pins, speeds);
    } else if (ir_values[5] < PRIMARY_IR_THRESHOLD) {
        speeds[0] = 30;
        speeds[1] = 50;
        move_motors(motor_phase_pins, speeds);
    }

}