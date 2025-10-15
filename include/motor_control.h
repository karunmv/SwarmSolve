#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

// GPIO Defines
#define M1_PWM 0
#define M1_DIR 1
#define M2_PWM 21
#define M2_DIR 20
#define ENC_PH_A_M1 11
#define ENC_PH_B_M1 10
#define ENC_PH_A_M2 19
#define ENC_PH_B_M2 18

// Directions
#define FORWARD 0x0A
#define REVERSE 0x0B
#define LEFT 0x0C
#define RIGHT 0x0D
#define STOP 0X0E

typedef struct{
    int forward[4];
    int reverse[4];
    int right[4];
    int left[4];
    int stop[4];
} directions;

// Function Prototypes
void read_encoder_values(uint32_t *gpio_pins);
void motor_setup(uint32_t *gpio_pins);
void robot_state(uint32_t *gpio_pins, uint8_t direction);

#endif
