#include "main.h"
#include "feature_detection.h"
#include "motor_control.h"
#include "QTRX-MD-NRC.h"

#include <rom/ets_sys.h>


static void find_line(uint32_t *ir_values, uint8_t *line_start, uint8_t *line_end, uint8_t *line_width) {

    *line_start  = NO_START;
    *line_end    = NO_END;
    *line_width  = 0;

    for (int i = 0; i < IR_PIN_COUNT; i++) {
        if (ir_values[i] > PRIMARY_IR_THRESHOLD) {

            if (i < *line_start) *line_start = i; // Update start of the line
            *line_end = i; // Updates the end of the line
            (*line_width)++; // Update width of the line
        }
    }
}

static uint8_t find_line_width(uint32_t *ir_values) {

    uint8_t line_width  = 0;
    for (int i = 0; i < IR_PIN_COUNT; i++) {
        if (ir_values[i] > PRIMARY_IR_THRESHOLD) {
            line_width++; // Update width of the line
        }
    }

    return line_width;
}

uint32_t calibrate_ir(uint32_t *ir_values){
    uint32_t max = 0;
    uint32_t min = 0xffff;
    uint32_t threshold = 0;

    for(int k=0; k<10; k++){
        for(int i=0; i<8; i++){
            if(ir_values[i] > max) max = ir_values[i];
            if(ir_values[i] < min) min = ir_values[i];
        }
    }

    threshold = max - ((max - min) * 0.1);

    //printf("Max: %ld Min: %ld Threshold: %ld", max,min,threshold);

    return threshold;

}

void update_feature_state(uint8_t *feature_state, uint32_t *ir_values) {

    uint8_t line_start, line_end, line_width;

    find_line(ir_values, &line_start, &line_end, &line_width);

    *feature_state = 0;

    if (line_width == 0) {
        *feature_state = DEAD_END;
        return;
    } else if (line_width <= 3 || (line_start > 0 && line_end < (IR_PIN_COUNT - 1))) {
        *feature_state = STRAIGHT_LINE;
        return;
    } 

    if (line_end == (IR_PIN_COUNT - 1))     *feature_state |= RIGHT_TURN;
    if (line_start == 0)                    *feature_state |= LEFT_TURN;
}

void check_straight(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t *feature_state) {
    
    /* Check if turn exists, and if so double check if the line continues straight */
    if ((*feature_state & RIGHT_TURN) || (*feature_state & LEFT_TURN)) {

        /* Move forward a tad */
        int8_t speeds[NUM_MOTORS] = {BASE_FORWARD_SPEED, BASE_FORWARD_SPEED};

        move_motors(motor_phase_pins, speeds);
        ets_delay_us(100000);
        speeds[0] = 0;
        speeds[1] = 0;
        move_motors(motor_phase_pins, speeds);
        
        /* Read IR Values */
        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
        uint8_t line_width;
        line_width = find_line_width(ir_values);

        /* Check if line continues or if it is a box */
        if ((line_width >= 3) && (line_width < (IR_PIN_COUNT - 1))) *feature_state |= STRAIGHT_LINE;
        else if (line_width == (IR_PIN_COUNT - 1)) *feature_state = END_OF_MAZE;

        *feature_state |= STRAIGHT_CHECKED;

    }
}

void follow_line(uint32_t *ir_values, uint32_t *motor_enable_pins, uint32_t *motor_phase_pins, uint32_t threshold) {
    
    int8_t speeds[NUM_MOTORS] = {BASE_FORWARD_SPEED, BASE_FORWARD_SPEED};

    int8_t error = 0;
    uint8_t old_i = 3;
    uint8_t kp = 5;
    uint8_t kd = 2;

    if ((ir_values[3]>threshold) && (ir_values[4]>threshold)){
        speeds[0] = BASE_FORWARD_SPEED;
        speeds[1] = BASE_FORWARD_SPEED;
        move_motors(motor_phase_pins, speeds);
    }
    else{
        for(int i=0; i<4; i++){
            if(ir_values[i]>threshold){
                error = (4-i)*((kp)) + ((i - old_i)*kd);
                old_i = i;
                speeds[0] -= error;
                move_motors(motor_phase_pins, speeds);

            } else if(ir_values[7-i]>threshold){
                error = (4-i)*((kp)) + ((i - old_i)*kd);
                old_i = i;
                speeds[1] -= error;
                move_motors(motor_phase_pins, speeds);
            }
        }
    }
}

void turn_right(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t feature_state) {

    int8_t speeds[NUM_MOTORS] = {BASE_TURN_SPEED, -BASE_TURN_SPEED};

    move_motors(motor_phase_pins, speeds); // Start turning right

    uint8_t line_width;

    // Add double check that it is on the line?

    /* Wait for robot to leave initial line */
    while(1) {
        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
        line_width = find_line_width(ir_values);

        if (line_width == 0) break;
    }

    /* Wait for robot to return to line */
    while(1) {
        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
        line_width = find_line_width(ir_values);

        if (line_width > 0) break;
    }

    speeds[0] = 20;
    speeds[1] = -20;
    move_motors(motor_phase_pins, speeds);

    /* Center robot on line */
    while(1) {
        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
        line_width = find_line_width(ir_values);

        if (ir_values[3] > PRIMARY_IR_THRESHOLD && ir_values[4] > PRIMARY_IR_THRESHOLD) break;
    }

    speeds[0] = 0;
    speeds[1] = 0;
    move_motors(motor_phase_pins, speeds);

}