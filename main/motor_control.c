#include "main.h"
#include "motor_control.h"

void read_encoder_values(uint32_t *gpio_pins){
    gpio_config_t encoder_config[4];

    for(int i=0; i<4; i++){
        
        encoder_config[i].pin_bit_mask = (1UL << gpio_pins[i]);
        encoder_config[i].mode = GPIO_MODE_INPUT;
        encoder_config[i].pull_up_en = GPIO_PULLUP_DISABLE;
        encoder_config[i].pull_down_en = GPIO_PULLDOWN_DISABLE;
        encoder_config[i].intr_type = GPIO_INTR_DISABLE;

        gpio_config(&(encoder_config[i]));
    }
}

void motor_setup(uint32_t *enable_pins, uint32_t *phase_pins) {

    /* Enable Pins (PWM) */
    for (int i = 0; i < NUM_MOTORS; i++) {

        // Prepare and then apply the LEDC PWM timer configuration
        ledc_timer_config_t ledc_timer = {
            .speed_mode       = LEDC_MODE,
            .duty_resolution  = LEDC_DUTY_RES,
            .timer_num        = i,
            .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
            .clk_cfg          = LEDC_AUTO_CLK
        };
        ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

        // Prepare and then apply the LEDC PWM channel configuration
        ledc_channel_config_t ledc_channel = {
            .speed_mode     = LEDC_MODE,
            .channel        = i,
            .timer_sel      = i,
            .intr_type      = LEDC_INTR_DISABLE,
            .gpio_num       = enable_pins[i],
            .duty           = 0, // Set duty to 0%
            .hpoint         = 0
        };
        ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    }

    /* Phase Pins */
    gpio_config_t motor_phase_config[NUM_MOTORS];
    for(int i = 0; i < NUM_MOTORS; i++) {
        motor_phase_config[i].pin_bit_mask = (1UL << phase_pins[i]);
        motor_phase_config[i].mode = GPIO_MODE_OUTPUT;
        motor_phase_config[i].pull_up_en = GPIO_PULLUP_DISABLE;
        motor_phase_config[i].pull_down_en = GPIO_PULLDOWN_DISABLE;
        motor_phase_config[i].intr_type = GPIO_INTR_DISABLE;

        gpio_config(&(motor_phase_config[i]));
    }

}

void move_motors(uint32_t *phase_pins, int8_t *speeds) {

    uint8_t direction;
    for (int i = 0; i < NUM_MOTORS; i++) {

        if (i == 0) speeds[i] *= -1; // Correct for mirrored motor

        direction = 1;
        if (speeds[i] < 0) {
            direction = 0;      // Check for sign in speed
            speeds[i] *= -1;    // Make speed positive
        }

        gpio_set_level(phase_pins[i], direction); // Direction

        ledc_set_duty(LEDC_MODE, i, (int)(speeds[i] * (8192 / 100))); // Speed
        ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, i));
    }
}


// void robot_state(uint32_t *phase_pins, uint8_t num_motors, uint32_t speed, uint8_t direction) {

//     // [Direction 1, Speed 1, Direction 2, Speed 2]
//     directions robot_go = {
//         {0, 100, 1, 100}, // forward
//         {1, 100, 0, 100}, // reverse
//         {0, 100, 0, 100}, // right
//         {1, 100,  1, 100}, // left
//         {0, 0, 0, 0} // stop
//     };

//     if (direction == FORWARD) {
//         for (int i = 0; i < num_motors; i++) {

//             gpio_set_level(phase_pins[i], robot_go.forward[2*i]); // Direction
            
//             ledc_set_duty(LEDC_MODE, i, (int)(speed * (8192 / 100) * (robot_go.forward[2*i + 1] / 100))); // Speed
//             ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, i));
//         }
//     } else if (direction == REVERSE) {
//         for (int i = 0; i < num_motors; i++) {

//             gpio_set_level(phase_pins[i], robot_go.reverse[2*i]); // Direction
            
//             ledc_set_duty(LEDC_MODE, i, (int)(speed * (8192 / 100) * (robot_go.reverse[2*i + 1] / 100))); // Speed
//             ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, i));
//         }
//     } else if (direction == RIGHT) {
//         for (int i = 0; i < num_motors; i++) {

//             gpio_set_level(phase_pins[i], robot_go.right[2*i]); // Direction
            
//             ledc_set_duty(LEDC_MODE, i, (int)(speed * (8192 / 100) * (robot_go.right[2*i + 1] / 100))); // Speed
//             ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, i));
//         }
//     } else if (direction == LEFT) {
//         for (int i = 0; i < num_motors; i++) {

//             gpio_set_level(phase_pins[i], robot_go.left[2*i]); // Direction
            
//             ledc_set_duty(LEDC_MODE, i, (int)(speed * (8192 / 100) * (robot_go.left[2*i + 1] / 100))); // Speed
//             ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, i));
//         }
//     } else if (direction == STOP) {
//         for (int i = 0; i < num_motors; i++) {

//             gpio_set_level(phase_pins[i], robot_go.stop[2*i]); // Direction
            
//             ledc_set_duty(LEDC_MODE, i, (int)(speed * (8192 / 100) * (robot_go.stop[2*i + 1] / 100))); // Speed
//             ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, i));
//         }
//     }
// }
