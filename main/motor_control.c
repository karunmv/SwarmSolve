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

void motor_setup(uint32_t *gpio_pins){    
    gpio_config_t motor_config[4];

    for(int i=0; i<4; i++){
        
        motor_config[i].pin_bit_mask = (1UL << gpio_pins[i]);
        motor_config[i].mode = GPIO_MODE_OUTPUT;
        motor_config[i].pull_up_en = GPIO_PULLUP_DISABLE;
        motor_config[i].pull_down_en = GPIO_PULLDOWN_DISABLE;
        motor_config[i].intr_type = GPIO_INTR_DISABLE;

        gpio_config(&(motor_config[i]));
    }
}

void robot_state(uint32_t *gpio_pins, uint8_t direction){
        directions robot_go = {
        {1,1,1,0},  // forward
        {1,0,1,1},  // reverse
        {1,1,1,1},  // right
        {1,0,1,0},  // left
        {0,0,0,0}   // stop
    };

        if(direction == FORWARD){
            for(int i=0; i<4; i++) gpio_set_level(gpio_pins[i], robot_go.forward[i]);
        } else if(direction == REVERSE) {
            for(int i=0; i<4; i++) gpio_set_level(gpio_pins[i], robot_go.reverse[i]);
        } else if(direction == LEFT) {
            for(int i=0; i<4; i++) gpio_set_level(gpio_pins[i], robot_go.left[i]);
        } else if(direction == RIGHT) {
            for(int i=0; i<4; i++) gpio_set_level(gpio_pins[i], robot_go.right[i]);
        } else {
            for(int i=0; i<4; i++) gpio_set_level(gpio_pins[i], robot_go.stop[i]);
        }
}
