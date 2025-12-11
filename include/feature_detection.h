#ifndef FEATURE_DETECTION_H
#define FEATURE_DETECTION_H


/* Defines */
#define NO_END                  0x00
#define NO_START                0xFF
#define CYCLES_TILL_CENTERED    5
#define MAX_CYCLES_TILL_CENTERED 125

/* Function prototypes*/

/**
 * @brief Finds threshold value for the line
 * @param ir_values Array of IR values that show both the line and a white background
 */
uint32_t calibrate_ir(uint32_t *ir_values);

/**
 * @brief Updates the feature state variable which holds information about what features are observed at a particular point in the maze
 * @param feature_state Pointer to the feature state variable which holds the current observed feature of the maze
 * @param ir_values Pointer to array of length IR_PIN_COUNT that holds the current values of the IR sensor array
 */
void update_feature_state(uint8_t *feature_state, uint32_t *ir_values);

/**
 * @brief When a turn is detected go forward to check if the line continues straight
 * @param ir_pins Pointer to array that holds the pins for the IR sensor array
 * @param ir_values Pointer to array that holds the IR sensor values
 * @param motor_phase_pins Pointer to array that holds the pin numbers of the motor phase pins
 * @param feature_state Pointer to the variable that holds the current feature state
 */
void check_straight(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t *feature_state, pcnt_unit_handle_t *pcnt_unit);

/**
 * @brief Follows a straight line
 * @param ir_values Pointer to array of IR sensor values
 * @param motor_phase_pins Pointer to array that holds the pin numbers of the motor phase pins
 */
void follow_line(uint32_t *ir_values, uint32_t *motor_phase_pins, uint32_t threshold);

/** 
 * @brief Centers the robot on the line while not moving forward
 * @param ir_pins Pointer to array that holds the pins for the IR sensor array
 * @param ir_values Pointer to array of IR sensor values
 * @param motor_phase_pins Pointer to array that holds the pin numbers of the motor phase pins
 */
void center_on_line_in_place(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins);

/** 
 * @brief Turns the robot right at an intersection until it reaches the next line
 * @param ir_pins Pointer to array that holds the pins for the IR sensor array
 * @param ir_values Pointer to array of IR sensor values
 * @param motor_phase_pins Pointer to array that holds the pin numbers of the motor phase pins
 */
void turn_right(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t feature_state);

/** 
 * @brief Turns the robot left at an intersection until it reaches the next line
 * @param ir_pins Pointer to array that holds the pins for the IR sensor array
 * @param ir_values Pointer to array of IR sensor values
 * @param motor_phase_pins Pointer to array that holds the pin numbers of the motor phase pins
 */
void turn_left(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t feature_state);

/** 
 * @brief Turns the robot around at an intesection or dead end, may skip a line if a right turn exists at that intersection
 * @param ir_pins Pointer to array that holds the pins for the IR sensor array
 * @param ir_values Pointer to array of IR sensor values
 * @param motor_phase_pins Pointer to array that holds the pin numbers of the motor phase pins
 */
void u_turn(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t feature_state);

/**
 * @brief Moves robot a fixed distance forward
 * @param motor_phase_pins Array of pins that are used to control the direction (phase) of the motors
 * @param pcnt_unit Pointer to the pulse counter
 * @param num_pulses Number of pulses the pulse count unit should detect before stopping
 */
void move_fixed_forward(uint32_t *motor_phase_pins, pcnt_unit_handle_t *pcnt_unit, uint32_t num_pulses);

#endif