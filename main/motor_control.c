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

void motor_setup(uint32_t *enable_pins, uint8_t num_enable_pins, uint32_t *phase_pins, uint8_t num_phase_pins) {

    /* Enable Pins */
    for (int i = 0; i < num_enable_pins; i++) {

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
    gpio_config_t motor_phase_config[num_phase_pins];
    for(int i = 0; i < num_phase_pins; i++) {
        motor_phase_config[i].pin_bit_mask = (1UL << phase_pins[i]);
        motor_phase_config[i].mode = GPIO_MODE_OUTPUT;
        motor_phase_config[i].pull_up_en = GPIO_PULLUP_DISABLE;
        motor_phase_config[i].pull_down_en = GPIO_PULLDOWN_DISABLE;
        motor_phase_config[i].intr_type = GPIO_INTR_DISABLE;

        gpio_config(&(motor_phase_config[i]));
    }

}

void robot_state(uint32_t *phase_pins, uint8_t num_motors, uint32_t speed, uint8_t direction) {
    directions robot_go = {
        {1, 0}, // foreward
        {0, 1}, // reverse
        {1, 1}, // right
        {0, 0}, // left
        {0, 0} // stop
    };


    uint8_t test[2] = {1, 1};

    if (direction == FORWARD) {
        for (int i = 0; i < num_motors; i++) {

            gpio_set_level(phase_pins[i], robot_go.forward[i]); // Direction
            
            ledc_set_duty(LEDC_MODE, i, (int)(speed * (8192 / 100))); // Speed
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, i));
        }
    } else if (direction == REVERSE) {
        for (int i = 0; i < num_motors; i++) {

            gpio_set_level(phase_pins[i], robot_go.reverse[i]); // Direction
            
            ledc_set_duty(LEDC_MODE, i, (int)(speed * (8192 / 100))); // Speed
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, i));
        }
    }
}
