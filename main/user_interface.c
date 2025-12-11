#include "main.h"
#include "user_interface.h"

static void button_press_callback(void *args) {
    
    uint8_t button_status = 1;
    
    xQueueSendFromISR(button_press_queue, &button_status, NULL);
}

void init_user_button(int button_gpio) {

    gpio_config_t button_config;

    // Set button GPIO config
    button_config.pin_bit_mask  = (1UL << button_gpio);
    button_config.mode          = GPIO_MODE_INPUT;
    button_config.pull_down_en  = GPIO_PULLDOWN_DISABLE;
    button_config.pull_up_en    = GPIO_PULLUP_ENABLE;
    button_config.intr_type     = GPIO_INTR_LOW_LEVEL;

    // Initialize gpio
    gpio_config(&button_config);

    // Set interrupt
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(button_gpio, button_press_callback, NULL);
}