#include "main.h"
#include "motor_control.h"

pcnt_unit_handle_t init_encoder(uint32_t *encoder_pins_A, uint32_t *encoder_pins_B) {
    
    // Set pulse count configuration
    pcnt_unit_config_t pcnt_config = {
        .high_limit = PCNT_HIGH_LIMIT,
        .low_limit  = PCNT_LOW_LIMIT,
    };
    pcnt_unit_handle_t pcnt_unit = NULL;
    ESP_ERROR_CHECK(pcnt_new_unit(&pcnt_config, &pcnt_unit));

    // Set pulse count glitch filter
    pcnt_glitch_filter_config_t filter_config = {
        .max_glitch_ns = 1000,
    };
    ESP_ERROR_CHECK(pcnt_unit_set_glitch_filter(pcnt_unit, &filter_config));

    // Configure teh pulse count channel A
    pcnt_chan_config_t chan_a_config = {
        .edge_gpio_num = encoder_pins_A[0],
        .level_gpio_num = encoder_pins_B[0],
    };
    pcnt_channel_handle_t pcnt_chan_a = NULL;
    ESP_ERROR_CHECK(pcnt_new_channel(pcnt_unit, &chan_a_config, &pcnt_chan_a));

    // Set action based on edge to increase counter when it detects an edge
    ESP_ERROR_CHECK(pcnt_channel_set_edge_action(pcnt_chan_a, PCNT_CHANNEL_EDGE_ACTION_DECREASE, PCNT_CHANNEL_EDGE_ACTION_HOLD));
    ESP_ERROR_CHECK(pcnt_channel_set_level_action(pcnt_chan_a, PCNT_CHANNEL_LEVEL_ACTION_KEEP, PCNT_CHANNEL_LEVEL_ACTION_INVERSE));

    // Enable pulse count unit and start counting
    ESP_ERROR_CHECK(pcnt_unit_enable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_clear_count(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_unit_start(pcnt_unit));

    return pcnt_unit;
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
        ESP_ERROR_CHECK( ledc_timer_config(&ledc_timer) );

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
        ESP_ERROR_CHECK( ledc_channel_config(&ledc_channel) );
    }

    /* Phase Pins */
    gpio_config_t motor_phase_config[NUM_MOTORS];
    for(int i = 0; i < NUM_MOTORS; i++) {
        motor_phase_config[i].pin_bit_mask = (1UL << phase_pins[i]);
        motor_phase_config[i].mode = GPIO_MODE_OUTPUT;
        motor_phase_config[i].pull_up_en = GPIO_PULLUP_DISABLE;
        motor_phase_config[i].pull_down_en = GPIO_PULLDOWN_DISABLE;
        motor_phase_config[i].intr_type = GPIO_INTR_DISABLE;

        ESP_ERROR_CHECK( gpio_config(&(motor_phase_config[i])) );
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

        ESP_ERROR_CHECK( gpio_set_level(phase_pins[i], direction) ); // Direction

        ESP_ERROR_CHECK( ledc_set_duty(LEDC_MODE, i, (int)(speeds[i] * (8192 / 100))) ); // Speed
        ESP_ERROR_CHECK( ledc_update_duty(LEDC_MODE, i) );
    }
}
