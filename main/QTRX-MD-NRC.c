#include "main.h"
#include "QTRX-MD-NRC.h"

#include <rom/ets_sys.h>
#include "esp_timer.h"

int start_time = 0;

static void ir_isr_handler(void *arg) {
    printf("IR Sensor Did Something: %lld\n", start_time - esp_timer_get_time());
}


void read_ir_sensor_array(int *gpio_pins, int count) {

    gpio_config_t IR_array_config[count];
    
    // Configure IR array for output
    for (int i = 0; i < count; i++) {
        
        IR_array_config[i].pin_bit_mask = (1UL << gpio_pins[i]);
        IR_array_config[i].mode = GPIO_MODE_OUTPUT;
        IR_array_config[i].pull_up_en = GPIO_PULLUP_DISABLE;
        IR_array_config[i].pull_down_en = GPIO_PULLDOWN_ENABLE;
        IR_array_config[i].intr_type = GPIO_INTR_DISABLE;

        gpio_config(&(IR_array_config[i]));

        // Set pin high
        gpio_set_level(gpio_pins[i], 1);
    }

    ets_delay_us(10);

    start_time = esp_timer_get_time();

    // gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3);

    for (int i = 0; i < count; i++) {
        IR_array_config[i].mode = GPIO_MODE_INPUT;
        // IR_array_config[i].intr_type = GPIO_INTR_NEGEDGE;

        gpio_config(&(IR_array_config[i]));
    }
    
    while(1) {
        if (gpio_get_level(gpio_pins[0]) == 0) {
            printf("THING: %lld\n", esp_timer_get_time() - start_time);
            break;
        }

        // vTaskDelay(pdMS_TO_TICKS(100));
    }


}