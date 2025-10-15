#include "main.h"
#include "QTRX-MD-NRC.h"

#include <rom/ets_sys.h>
#include "esp_timer.h"

uint32_t start_time = 0;
uint32_t time_elapsed;


void read_ir_sensor_array(uint32_t *gpio_pins, uint32_t *IR_sensor_readings, uint32_t count) {

    // Initialize IR sensor array
    gpio_config_t IR_array_config[count];
    
    // Configure IR array for output
    for (uint32_t i = 0; i < count; i++) {

        IR_sensor_readings[i] = MAX_IR_READTIME;
        
        IR_array_config[i].pin_bit_mask = (1UL << gpio_pins[i]);
        IR_array_config[i].mode = GPIO_MODE_OUTPUT;
        IR_array_config[i].pull_up_en = GPIO_PULLUP_DISABLE;
        IR_array_config[i].pull_down_en = GPIO_PULLDOWN_DISABLE;
        IR_array_config[i].intr_type = GPIO_INTR_DISABLE;

        gpio_config(&(IR_array_config[i]));

        // Set pin high
        gpio_set_level(gpio_pins[i], 1);
    }

    // Delay 10 microseconds
    ets_delay_us(10);

    // Set start time for sensor reading
    start_time = esp_timer_get_time();

    // Configure IR array as input (high impedence)
    for (uint32_t i = 0; i < count; i++) {
        IR_array_config[i].mode = GPIO_MODE_INPUT;
        
        gpio_config(&(IR_array_config[i]));
    } 
    
    int values_received = 0;
    time_elapsed = esp_timer_get_time() - start_time;

    // Wait until pin is read as low then print time elapsed
    while(time_elapsed < MAX_IR_READTIME) {
        time_elapsed = esp_timer_get_time() - start_time;

        for (int i = 0; i < count; i++) {
            if ((gpio_get_level(gpio_pins[i]) == 0) && (time_elapsed < IR_sensor_readings[i])) {
                IR_sensor_readings[i] = time_elapsed;

                values_received++;
                if (values_received == count) break;
            }
        }
    }
}