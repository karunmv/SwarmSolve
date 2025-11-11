#include "main.h"
#include "feature_detection.h"
#include "motor_control.h"
#include "QTRX-MD-NRC.h"

#include <rom/ets_sys.h>

static int calculate_line_error(uint32_t *ir_values) {
    int error = 0;

    // TODO Fix hardcoded values
    for (int i=0; i<4; i++) {
        if (ir_values[i] > PRIMARY_IR_THRESHOLD) {
            error = (3 - i);
            break;
        }
    }
    for (int i = 7; i > 3; i--) {
        if (ir_values[i] > PRIMARY_IR_THRESHOLD) {
            if (error < (i - 4)) {
                error = (4 - i);
            }
            break;
        }
    }

    return error;
}

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

void check_straight(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t *feature_state, pcnt_unit_handle_t *pcnt_unit) {

    /* Check if turn exists, and if so double check if the line continues straight */
    if ((*feature_state & RIGHT_TURN) || (*feature_state & LEFT_TURN)) {

        /* Variables */
        int8_t speeds[NUM_MOTORS] = {BASE_FORWARD_SPEED, BASE_FORWARD_SPEED};
        int pulse_count;


        /* Move forward a tad */
        // Start Moving
        move_motors(motor_phase_pins, speeds);

        // Wait for certain pules count
        ESP_ERROR_CHECK(pcnt_unit_clear_count(*pcnt_unit));
        pcnt_unit_get_count(*pcnt_unit, &pulse_count);

        while (pulse_count < CHECK_STRAIGHT_PULSES) {
            pcnt_unit_get_count(*pcnt_unit, &pulse_count);
        }
        
        // Stop moving
        speeds[0] = 0;
        speeds[1] = 0;
        move_motors(motor_phase_pins, speeds);
        
        /* Read IR Values */
        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
        uint8_t line_width;
        line_width = find_line_width(ir_values);

        /* Check if line continues or if it is a box */
        if ((line_width > 0) && (line_width < (IR_PIN_COUNT - 1))) *feature_state |= STRAIGHT_LINE;
        else if (line_width == IR_PIN_COUNT) *feature_state = END_OF_MAZE;

        *feature_state |= STRAIGHT_CHECKED;

    }
}

void follow_line(uint32_t *ir_values, uint32_t *motor_phase_pins, uint32_t threshold) {
    
    int8_t speeds[NUM_MOTORS] = {BASE_FORWARD_SPEED, BASE_FORWARD_SPEED};

    // int8_t error = 0;
    int error;
    static int last_error, error_sum;
    float correction;
    float kp = 5;
    float kd = 0;
    float ki = 0.2;

    error = calculate_line_error(ir_values);
    error_sum += error;
    
    correction = kp * error + kd * (last_error - error) + ki * error_sum;

    speeds[0] = BASE_FORWARD_SPEED - correction;
    speeds[1] = BASE_FORWARD_SPEED + correction;
    move_motors(motor_phase_pins, speeds);

    last_error = error;
}

void center_on_line_in_place(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins) {
    
    uint16_t watchdog_count = 0;
    uint16_t centered_count = 0;
    uint8_t line_width;
    int8_t speeds[NUM_MOTORS] = {0, 0};
    int error = 0;
    int last_error = 0;
    float correction = 0;
    float kp = 9;
    float kd = 1.1;


    while(1) {
        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);

        error = calculate_line_error(ir_values);

        correction = kp * error + kd * (last_error - error);

        last_error = error;

        speeds[0] = -correction;
        speeds[1] = correction;
        move_motors(motor_phase_pins, speeds);

        // Wait until the robot has 0 error for a certain number of cycles before returning
        if (error == 0) centered_count++;
        else centered_count = 0;
        watchdog_count++;

        if (centered_count >= CYCLES_TILL_CENTERED) break;
        if (watchdog_count >= MAX_CYCLES_TILL_CENTERED) break;
    }
}

void turn_right(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t feature_state) {

    uint8_t line_width;
    int8_t speeds[NUM_MOTORS] = {BASE_TURN_SPEED, -BASE_TURN_SPEED};

    move_motors(motor_phase_pins, speeds); // Start turning right

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

    center_on_line_in_place(ir_pins, ir_values, motor_phase_pins);
}

void turn_left(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t feature_state) {

    uint8_t line_width;
    int8_t speeds[NUM_MOTORS] = {-BASE_TURN_SPEED, BASE_TURN_SPEED};

    move_motors(motor_phase_pins, speeds); // Start turning right

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

    center_on_line_in_place(ir_pins, ir_values, motor_phase_pins);
}

void u_turn(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t feature_state) {

    uint8_t line_width;
    int8_t speeds[NUM_MOTORS] = {BASE_TURN_SPEED, -BASE_TURN_SPEED};

    move_motors(motor_phase_pins, speeds); // Start turning right

    /* Run twice if there is a right turn present at the intersection to skip over the extra line */
    for (int i = 0; i <= (feature_state & RIGHT_TURN); i++) {
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
    }

    center_on_line_in_place(ir_pins, ir_values, motor_phase_pins);
}

