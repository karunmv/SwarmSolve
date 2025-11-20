#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "driver/ledc.h"


// GPIO Defines
#define M1_PWM 0
#define M1_DIR 1
#define M2_PWM 21
#define M2_DIR 20
#define ENC_PH_A_M1 11
#define ENC_PH_B_M1 10
#define ENC_PH_A_M2 19
#define ENC_PH_B_M2 18

// PWM Defines
#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE
#define LEDC_CHANNEL            LEDC_CHANNEL_0
#define LEDC_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz

// Directions
#define FORWARD 0x0A
#define REVERSE 0x0B
#define LEFT 0x0C
#define RIGHT 0x0D
#define STOP 0X0E

// Pulse Count Defines
#define PCNT_HIGH_LIMIT INT16_MAX
#define PCNT_LOW_LIMIT  INT16_MIN

typedef struct{
    int forward[4];
    int reverse[4];
    int right[4];
    int left[4];
    int stop[4];
} directions;

/* Function Prototypes */

/**
 * @brief Placeholder for function that will read the motor encodor values
 */
pcnt_unit_handle_t init_encoder(uint32_t *encoder_pins_A, uint32_t *encoder_pins_B);

/**
 * @brief Initializes motor driver GPIO pins and pwm clocks
 * @param enable_pins Array of size NUM_MOTORS that holds the GPIO pins that are connected to the enable pins of the motor control board
 * @param enable_pins Array of size NUM_MOTORS that holds the GPIO pins that are connected to the phase pins of the motor control board
 */
void motor_setup(uint32_t *enable_pins, uint32_t *phase_pins);

/**
 * @brief Moves each motor individually at the speed provided (-100 -> 100) negative values are backwards
 * @param phase_pins Array of lengh NUM_MOTORS that defines the GPIO pins used for the phase (direction) of the motor
 * @param speeeds Array of length NUM_MOTORS that defines the speeds for each individual motor [LEFT MOTOR, RIGHT MOTOR]
 */
void move_motors(uint32_t *phase_pins, int8_t *speeds);

#endif
