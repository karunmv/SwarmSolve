#ifndef QTRX_MD_NRC_H
#define QTRX_MD_NRC_H

#define MAX_IR_READTIME 10000

#define IR_PIN_1 17
#define IR_PIN_2 16 
#define IR_PIN_3 3
#define IR_PIN_4 2
#define IR_PIN_5 4
#define IR_PIN_6 5
#define IR_PIN_7 6
#define IR_PIN_8 7


/**
 * @brief Gets a reading from an IR sensor array
 * @param gpio_pins Integer array of GPIO pins used by the sensor array
 * @param count Number of sensors in array
 * @return Array of sensor values of size count
 */
void read_ir_sensor_array(uint32_t *gpio_pins, uint32_t count);

#endif