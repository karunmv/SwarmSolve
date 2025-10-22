#ifndef QTRX_MD_NRC_H
#define QTRX_MD_NRC_H

#define MAX_IR_READTIME 10000


#define IR_PIN_COUNT 8
#define IR_PIN_1 17
#define IR_PIN_2 16 
#define IR_PIN_3 3
#define IR_PIN_4 2
#define IR_PIN_5 4
#define IR_PIN_6 5
#define IR_PIN_7 6
#define IR_PIN_8 7


/**
 * @brief Gets a reading from an IR sensor array, higher values = Less reflectance (more black).
 * @details Sets line high, then sets line as an input and measures how long it takes to drop to zero
 * @param gpio_pins Pointer to tnteger array of GPIO pins used by the sensor array
 * @param IR_sensor_readings Pointer to integer array to be used to store IR sensor readings in
 * @param count Number of sensors in array
 */
void read_ir_sensor_array(uint32_t *gpio_pins, uint32_t *IR_sensor_readings, uint32_t count);

#endif