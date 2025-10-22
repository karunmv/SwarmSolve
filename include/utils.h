#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>

/* Function Prototypes */

/**
 * @brief Prints the ir values stored in an array of length IR_PIN_COUNT in a readable format
 * @param ir_values Array of length IR_PIN_COUNT containing the ir sensor values
 */
void print_IR_values(uint32_t *ir_values);

#endif