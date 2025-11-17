#include "main.h"
#include "esp_now_comms.h"
#include "QTRX-MD-NRC.h"
#include "motor_control.h"
#include "feature_detection.h"
#include "navigation_control.h"
#include "utils.h"
#include "user_interface.h"

QueueHandle_t path_transfer_queue   = NULL;
QueueHandle_t button_press_queue    = NULL;

void app_main(void) {

    /* PIN ARRAYS */
    uint32_t ir_pins[IR_PIN_COUNT] = {IR_PIN_1, IR_PIN_2, IR_PIN_3, IR_PIN_4, IR_PIN_5, IR_PIN_6, IR_PIN_7, IR_PIN_8};
    uint32_t motor_enable_pins[NUM_MOTORS] = {M1_PWM, M2_PWM};
    uint32_t motor_phase_pins[NUM_MOTORS] = {M1_DIR, M2_DIR};
    uint32_t motor_encoder_A_pins[NUM_MOTORS] = {ENC_PH_A_M1, ENC_PH_A_M2};
    uint32_t motor_encoder_B_pins[NUM_MOTORS] = {ENC_PH_B_M1, ENC_PH_B_M2};
    uint32_t button_pin = BUTTON_PIN;
    
    /* Variable declarations */
    uint32_t ir_values[IR_PIN_COUNT];
    uint8_t feature_state = 0;
    uint8_t movement_state = 0;
    int8_t speeds[NUM_MOTORS] = {0, 0};
    uint8_t following_path = 0;
    uint8_t local_path[NUM_MAP_FEATURES] = {0};
    uint8_t solved_path[NUM_MAP_FEATURES] = {0};
    uint8_t final_path[NUM_MAP_FEATURES] = {0};
    pcnt_unit_handle_t pcnt_unit;
    int pulse_count = 0;
    uint8_t data[NUM_MAP_FEATURES + 1];
    uint8_t started = 0;
    uint8_t from_button;

    /* Debugging */
    uint8_t last_state = 0;

    /* PATH TESTING */
    // uint8_t local_path[NUM_MAP_FEATURES] = {TURNING_RIGHT, TURNING_LEFT, U_TURN, TURNING_RIGHT, TURNING_RIGHT, TURNING_RIGHT, U_TURN};
    // uint8_t solved_path[NUM_MAP_FEATURES] = {TURNING_RIGHT, TURNING_LEFT, U_TURN, TURNING_RIGHT, TURNING_RIGHT, TURNING_RIGHT, U_TURN, GOING_STRAIGHT, U_TURN, TURNING_RIGHT, TURNING_RIGHT, TURNING_RIGHT, STOPPED};

    // generate_shortest_path(local_path, solved_path, final_path);

    // printf("PATH LIST: ");
    // for (int i = 0; i < NUM_MAP_FEATURES; i++) {
    //     printf("%d-", i);
    //     print_movement_state(final_path[i]);
    // }
    // printf("\n");

    // while(1) {
    //     vTaskDelay(pdMS_TO_TICKS(1000));
    // }


    /* Communication Initialization */
    wifi_sta_init();
    esp_broadcast_setup();

    /* Motor Pin Initialization */
    motor_setup(motor_enable_pins, motor_phase_pins);

    /* Encoder Initialization */
    pcnt_unit = init_encoder(motor_encoder_A_pins, motor_encoder_B_pins);

    /* Button Initialization */
    init_user_button(button_pin);

    /* Create Queues */
    path_transfer_queue = xQueueCreate(DEFAULT_QUEUE_SIZE, sizeof(uint8_t) * (NUM_MAP_FEATURES + 1));
    button_press_queue  = xQueueCreate(DEFAULT_QUEUE_SIZE, sizeof(uint8_t));


    // printf("Calibrating...\n");
    // vTaskDelay(pdMS_TO_TICKS(3000));
    // read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
    // uint32_t threshold = calibrate_ir(ir_values);

    // printf("Calibrated Successfully!\n");

    // int counter = 0;

    /* Main Loop */
    while(1) {

        if (xQueueReceive(button_press_queue, &from_button, 5) && !started) {

            if (from_button) {
                send_started();
                started = 1;
            } else {
                vTaskDelay(pdMS_TO_TICKS(3000));
                started = 1;
            }
        }

        /* Receive data from other robot */
        if ((following_path == 0) && xQueueReceive(path_transfer_queue, data, 5)) {

            // Strip packet type from first byte of data
            uint8_t packet_type = data[0];

            if (packet_type == PATH_PACKET) {
                memcpy(solved_path, &(data[1]), NUM_MAP_FEATURES);

                generate_shortest_path(local_path, solved_path, final_path);
                following_path = 1;

            }
        }

        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);
        // print_IR_values(ir_values);
        // send_ir_values(ir_values);

        if (started) {
            update_feature_state(&feature_state, ir_values);

            if (feature_state != last_state) {
                send_ir_values(ir_values);
            }

            check_straight(ir_pins, ir_values, motor_phase_pins, &feature_state, &pcnt_unit);

            update_movement_state(feature_state, &movement_state, local_path, following_path);

            if (feature_state != last_state) {
                send_state(feature_state, movement_state);
                last_state = feature_state;
            }

            // threshold = calibrate_ir(ir_values);

            // if (counter == 10) {
                // send_state(feature_state, movement_state);
                // send_ir_values(ir_values);
            //     counter = 0;
            // }
            // counter++;

            // center_on_line_in_place(ir_pins, ir_values, motor_phase_pins);

            if (movement_state == GOING_STRAIGHT) {
                follow_line(ir_values, motor_phase_pins, PRIMARY_IR_THRESHOLD);
            } else if (movement_state == STOPPED) {
                move_motors(motor_phase_pins, speeds);
                prune_map(local_path);
                while(1) {
                    send_path(local_path, feature_state);
                    vTaskDelay(pdMS_TO_TICKS(500));
                }
            } else if (movement_state == TURNING_RIGHT) {
                turn_right(ir_pins, ir_values, motor_phase_pins, feature_state);
            } else if (movement_state == TURNING_LEFT) {
                turn_left(ir_pins, ir_values, motor_phase_pins, feature_state);
            } else if (movement_state == U_TURN) {
                u_turn(ir_pins, ir_values, motor_phase_pins, feature_state);
            }
            

            // send_path(local_path, feature_state);


            // print_feature_state(feature_state);
            // print_movement_state(movement_state);
            // print_IR_values(ir_values);
        }

        // vTaskDelay(pdMS_TO_TICKS(10));
    }
}