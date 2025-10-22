#ifndef FEATURE_DETECTION_H
#define FEATURE_DETECTION_H


/* Defines */
#define PRIMARY_IR_THRESHOLD    1500
#define NO_END                  0x00
#define NO_START                0xFF

/* Function prototypes*/

/**
 * 
 */
void update_feature_state(uint8_t *feature_state, uint32_t *ir_values);


void follow_line(uint32_t *ir_values, uint32_t *motor_enable_pins, uint32_t *motor_phase_pins);


#endif