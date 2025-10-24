#ifndef FEATURE_DETECTION_H
#define FEATURE_DETECTION_H


/* Defines */
#define PRIMARY_IR_THRESHOLD    1250
#define NO_END                  0x00
#define NO_START                0xFF

/* Function prototypes*/

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
void check_straight(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t *feature_state);

/**
 * @brief Follows a straight line
 * @param ir_values Pointer to array of IR sensor values
 * @param motor_enable_pins Pointer to array that holds the pin numbers of the motor enable pins of the motor driver
 * @param motor_phase_pins Pointer to array that holds the pin numbers of the motor phase pins
 */
void follow_line(uint32_t *ir_values, uint32_t *motor_enable_pins, uint32_t *motor_phase_pins, uint32_t threshold);

uint32_t calibrate_ir(uint32_t *ir_values);


void turn_right(uint32_t *ir_pins, uint32_t *ir_values, uint32_t *motor_phase_pins, uint8_t feature_state);

#endif