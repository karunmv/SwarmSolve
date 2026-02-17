#include "main.h"
#include "esp_now_comms.h"
#include "utils.h"

#include <stdio.h>

QueueHandle_t path_transfer_queue = NULL;

void app_main(void) {
    
    /* Communication Initialization */
    wifi_sta_init();
    esp_broadcast_setup();

    /* Queue setup */
    path_transfer_queue = xQueueCreate(5, sizeof(uint8_t) * (NUM_MAP_FEATURES + 1));

    uint8_t data[NUM_MAP_FEATURES + 1] = {0};
    uint8_t *path;

    while (1) {

        /* Print received path from queue */
        if (xQueueReceive(path_transfer_queue, data, 5)) {

            // Strip packet type from first byte of data
            uint8_t packet_type = data[0];
            path = &(data[1]);

            if (packet_type == PATH_PACKET) {

                printf("PATH LIST (From Queue): ");
                for (int i = 0; i < NUM_MAP_FEATURES; i++) {
                    print_movement_state(path[i]);
                }
                printf("\n");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}