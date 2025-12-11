#include "main.h"
#include "feature_detection.h"
#include "motor_control.h"
#include "QTRX-MD-NRC.h"

#include <rom/ets_sys.h>

/**
 * @brief Calculates the error from the center of the line of the IR sensor
 * @param ir_values Array of ir values
 */
static int calculate_line_error(uint32_t *ir_values) {
    int error = 0;

    // Check for positive error from one half of IR array
    for (int i=0; i < (IR_PIN_COUNT / 2); i++) {
        if (ir_values[i] > PRIMARY_IR_THRESHOLD) {
            error = (((IR_PIN_COUNT / 2) - 1) - i);
            break;
        }
    }

    // Check for negative error from the other half of the IR arrya
    for (int i = (IR_PIN_COUNT - 1); i > ((IR_PIN_COUNT / 2) - 1); i--) {
        if (ir_values[i] > PRIMARY_IR_THRESHOLD) {
            if (error < (i - (IR_PIN_COUNT / 2))) {
                error = ((IR_PIN_COUNT / 2) - i);
            }
            break;
        }
    }

    return error;
}

/**
 * @brief Finds the start, end, and width of the line based on the IR sensor values
 * @param ir_values Array of ir values
 * @param line_start Pointer to line start variable, gets modified in place
 * @param line_end Pointer to line end variable, gets modified in place
 * @param line_width Pointer to line width variable, gets modified in place
 */
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

/**
 * @brief Returns width of the line
 * @param ir_values Array of ir values
 */
static uint8_t find_line_width(uint32_t *ir_values) {

    uint8_t line_width  = 0;
    for (int i = 0; i < IR_PIN_COUNT; i++) {
        if (ir_values[i] > PRIMARY_IR_THRESHOLD) {
            line_width++; // Update width of the line
        }
    }

    return line_width;
}


void move_fixed_forward(uint32_t *motor_phase_pins, pcnt_unit_handle_t *pcnt_unit, uint32_t num_pulses) {
    /* Variables */
    int8_t speeds[NUM_MOTORS] = {BASE_FORWARD_SPEED, BASE_FORWARD_SPEED + FORWARD_TUNE};
    int pulse_count;

    /* Move forward a tad */
    // Start Moving
    move_motors(motor_phase_pins, speeds);

    // Wait for certain pules count
    ESP_ERROR_CHECK( pcnt_unit_clear_count(*pcnt_unit) );
    ESP_ERROR_CHECK( pcnt_unit_get_count(*pcnt_unit, &pulse_count) );

    while (pulse_count < num_pulses) {
        ESP_ERROR_CHECK( pcnt_unit_get_count(*pcnt_unit, &pulse_count) );
    }

    speeds[0] = 0;
    speeds[1] = 0;
    move_motors(motor_phase_pins, speeds);
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
    } else if (line_width <= 2 || (line_start > 0 && line_end < (IR_PIN_COUNT - 1))) {
        *feature_state = STRAIGHT_LINE;
        return;
    }

    // Check for left/right
    if (line_end == (IR_PIN_COUNT - 1))     *feature_state |= RIGHT_TURN;
    if (line_start == 0)                    *feature_state |= LEFT_TURN;
}

void check_straight(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t *feature_state, pcnt_unit_handle_t *pcnt_unit) {

    /* Check if turn exists, and if so double check if the line continues straight */
    if ((*feature_state & RIGHT_TURN) || (*feature_state & LEFT_TURN)) {

        /* Variables */
        int8_t speeds[NUM_MOTORS] = {BASE_FORWARD_SPEED, BASE_FORWARD_SPEED + FORWARD_TUNE};
        uint8_t line_start, line_end, line_width;


        speeds[0] = 0;
        speeds[1] = 0;
        move_motors(motor_phase_pins, speeds);
        ets_delay_us(1000);

        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
        find_line(ir_values, &line_start, &line_end, &line_width);

        // Check for left/right
        if (line_end == (IR_PIN_COUNT - 1))     *feature_state |= RIGHT_TURN;
        if (line_start == 0)                    *feature_state |= LEFT_TURN;


        move_fixed_forward(motor_phase_pins, pcnt_unit, 600);
        
        /* Read IR Values */
        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
        line_width = find_line_width(ir_values);

        /* Check if line continues or if it is a box */
        if ((line_width > 0) && (line_width < (IR_PIN_COUNT - 1))) {
            *feature_state |= STRAIGHT_LINE;
            center_on_line_in_place(ir_pins, ir_values, motor_phase_pins);
        } else if (line_width == IR_PIN_COUNT) *feature_state = END_OF_MAZE;

        *feature_state |= STRAIGHT_CHECKED;

    }
}

void follow_line(uint32_t *ir_values, uint32_t *motor_phase_pins, uint32_t threshold) {
    
    int8_t speeds[NUM_MOTORS] = {BASE_FORWARD_SPEED, BASE_FORWARD_SPEED + FORWARD_TUNE};

    // int8_t error = 0;
    int error;
    static int last_error, error_sum;
    float correction;
    float kp = 3.0;
    float kd = 1.0;
    float ki = 0.1;

    error = calculate_line_error(ir_values);
    error_sum += error;
    
    correction = kp * error + kd * (error - last_error) + ki * error_sum;

    speeds[0] = BASE_FORWARD_SPEED - correction;
    speeds[1] = BASE_FORWARD_SPEED + correction + FORWARD_TUNE;
    move_motors(motor_phase_pins, speeds);

    last_error = error;
}

void center_on_line_in_place(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins) {
    
    uint16_t watchdog_count = 0;
    uint16_t centered_count = 0;
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

    turn_right(ir_pins, ir_values, motor_phase_pins, feature_state);

    if (feature_state & RIGHT_TURN) {
        turn_right(ir_pins, ir_values, motor_phase_pins, feature_state);
    }
}

