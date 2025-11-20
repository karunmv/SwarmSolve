#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/* Function Prototypes */

/**
 * @brief Prints the ir values stored in an array of length IR_PIN_COUNT in a readable format
 * @param ir_values Array of length IR_PIN_COUNT containing the ir sensor values
 */
void print_IR_values(uint32_t *ir_values);

/**
 * @brief Prints the feature state in plain text
 * @param feature_state Feature state byte
 */
void print_feature_state(uint8_t feature_state);

/**
 * @brief Prints the movement state in plain text
 * @param feature_state Movement state byte
 */
void print_movement_state(uint8_t movement_state);

#endif