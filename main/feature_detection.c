#include "main.h"
#include "feature_detection.h"
#include "motor_control.h"
#include "QTRX-MD-NRC.h"


void update_feature_state(uint8_t *feature_state, uint32_t *ir_values) {

    uint8_t line_start  = NO_START;
    uint8_t line_end    = NO_END;
    uint8_t line_width  = 0;

    for (int i = 0; i < IR_PIN_COUNT; i++) {

        if (ir_values[i] > PRIMARY_IR_THRESHOLD) {

            if (i < line_start) line_start = i; // Update start of the line
            line_end = i; // Updates the end of the line
            line_width++; // Update width of the line
        }
    }

    *feature_state = 0;

    if (line_width == 0) {
        *feature_state |= DEAD_END;
        return; // Won't be any other states
    } else if (line_width == 2 || (line_start > 0 && line_end < (IR_PIN_COUNT - 1))) {
        *feature_state |= STRAIGHT_LINE;
        return;
    } 

    if (line_end == (IR_PIN_COUNT - 1))     *feature_state |= RIGHT_TURN;
    if (line_start == 0)                    *feature_state |= LEFT_TURN;

    
    

    else *feature_state = END_OF_MAZE;
}

void follow_line(uint32_t *ir_values, uint32_t *motor_enable_pins, uint32_t *motor_phase_pins) {
    
    int8_t speeds[NUM_MOTORS] = {50, 50};

    uint8_t error = 0;

    for(int i=0; i<4; i++){
        if(ir_values[i]>PRIMARY_IR_THRESHOLD){
            if(i==3) speeds[1]=50;
            else{
                error = (4-i)*((ir_values[i]-PRIMARY_IR_THRESHOLD)/200);
                speeds[1] += error;
            }
            move_motors(motor_phase_pins, speeds);

        } else if(ir_values[7-i]>PRIMARY_IR_THRESHOLD){
            if(i==4) speeds[0]=50;
            else{
                error = (4-i)*((ir_values[7-i]-PRIMARY_IR_THRESHOLD)/200);
                speeds[0] += error;
            }
            move_motors(motor_phase_pins, speeds);
        } else{
            speeds[0]=50;
            speeds[1]=50;
        }
    }

    // if (ir_values[7] > PRIMARY_IR_THRESHOLD) {
    //     speeds[0] = 50;
    //     speeds[1] = 35;
    //     move_motors(motor_phase_pins, speeds);    
    // } else if (ir_values[0] > PRIMARY_IR_THRESHOLD) {
    //     speeds[0] = 35;
    //     speeds[1] = 50;
    //     move_motors(motor_phase_pins, speeds);    
    // } else if (ir_values[6] > PRIMARY_IR_THRESHOLD) {
    //     speeds[0] = 50;
    //     speeds[1] = 40;
    //     move_motors(motor_phase_pins, speeds);    
    // } else if (ir_values[1] > PRIMARY_IR_THRESHOLD) {
    //     speeds[0] = 40;
    //     speeds[1] = 50;
    //     move_motors(motor_phase_pins, speeds);    
    // } else if (ir_values[5] > PRIMARY_IR_THRESHOLD) {
    //     speeds[0] = 50;
    //     speeds[1] = 45;
    //     move_motors(motor_phase_pins, speeds);    
    // } else if (ir_values[2] > PRIMARY_IR_THRESHOLD) {
    //     speeds[0] = 45;
    //     speeds[1] = 50;
    //     move_motors(motor_phase_pins, speeds);    
    // } else {
    //     speeds[0] = 50;
    //     speeds[1] = 50;
    //     move_motors(motor_phase_pins, speeds);
    // }

}