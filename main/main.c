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

    /* ESPNOW Info */
    uint8_t peer_mac[6] = PEER_MAC;
    uint8_t local_mac[6];

    /* PIN ARRAYS */
    uint32_t ir_pins[IR_PIN_COUNT] = {IR_PIN_1, IR_PIN_2, IR_PIN_3, IR_PIN_4, IR_PIN_5, IR_PIN_6, IR_PIN_7, IR_PIN_8};
    uint32_t motor_enable_pins[NUM_MOTORS] = {M1_PWM, M2_PWM};
    uint32_t motor_phase_pins[NUM_MOTORS] = {M1_DIR, M2_DIR};
    uint32_t motor_encoder_A_pins[NUM_MOTORS] = {ENC_PH_A_M1, ENC_PH_A_M2};
    uint32_t motor_encoder_B_pins[NUM_MOTORS] = {ENC_PH_B_M1, ENC_PH_B_M2};
    uint32_t button_pin = BUTTON_PIN;
    
    /* Variable declarations */
    uint32_t ir_values[IR_PIN_COUNT];               // Holds IR sensor values
    uint8_t feature_state = 0;                      // Holds current feature state
    uint8_t movement_state = 0;                     // Holds current movement state
    int8_t speeds[NUM_MOTORS] = {0, 0};             // Holds current motor speeds
    uint8_t following_path = 0;                     // Shows if robot is following a path or exploring
    uint8_t second_to_finish = 0;                   // Flagged only if second robot finishes maze
    uint8_t local_path[NUM_MAP_FEATURES] = {0};     // Stores robot path
    uint8_t solved_path[NUM_MAP_FEATURES] = {0};    // Stores solved path received from other robot
    pcnt_unit_handle_t pcnt_unit;                   // Pulse counter for encoders
    uint8_t data[NUM_MAP_FEATURES + 1];             // Data buffer for receiving esp-now packets
    uint8_t started = 0;                            // Tells robot when to start moving
    uint8_t from_button;                            // Checks if local button is pressed

    /* Debugging */
    uint8_t last_state = 0;

    /* Communication Initialization */
    wifi_sta_init();
    esp_setup(peer_mac, local_mac);

    /* Motor Pin Initialization */
    motor_setup(motor_enable_pins, motor_phase_pins);

    /* Encoder Initialization */
    pcnt_unit = init_encoder(motor_encoder_A_pins, motor_encoder_B_pins);

    /* Button Initialization */
    init_user_button(button_pin);

    /* Create Queues */
    path_transfer_queue = xQueueCreate(DEFAULT_QUEUE_SIZE, sizeof(uint8_t) * (NUM_MAP_FEATURES + 1));
    button_press_queue  = xQueueCreate(DEFAULT_QUEUE_SIZE, sizeof(uint8_t));

    /* Main Loop */
    while(1) {

        /* Check if button press has been received on this robot, or from the other robot */
        if (xQueueReceive(button_press_queue, &from_button, 5) && !started) {

            // If from local button start immediatly
            if (from_button) {
                send_started(peer_mac);
                started = 1;

            // If from other robot start after 3 seconds
            } else {

                // Move forward a little to show it's alive
                move_fixed_forward(motor_phase_pins, &pcnt_unit, 10);
                
                // Wait 3 seconds then start
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

                generate_shortest_path(local_path, solved_path);
                following_path = 1;
                second_to_finish = 1;

            }
        }

        read_ir_sensor_array(ir_pins, ir_values, IR_PIN_COUNT);

        // Wait until the button has been pressed
        if (started) {
            // Update with visisble features
            update_feature_state(&feature_state, ir_values);

            // For debugging/status purposes
            if (feature_state != last_state) {
                send_ir_values(ir_values, peer_mac);
            }
            
            // Move forward to see if line continues if at intersection
            check_straight(ir_pins, ir_values, motor_phase_pins, &feature_state, &pcnt_unit);

            // Update how the robot should move
            update_movement_state(feature_state, &movement_state, local_path, &following_path);

            // For debugging/status purposes
            if (feature_state != last_state) {
                send_state(feature_state, movement_state, peer_mac);
                last_state = feature_state;
            }

            // Navigate feature depending on what the state is
            if (movement_state == GOING_STRAIGHT) {
                follow_line(ir_values, motor_phase_pins, PRIMARY_IR_THRESHOLD);
            } else if (movement_state == STOPPED) {

                // Stop moving and send the pruned path to the other robot
                move_motors(motor_phase_pins, speeds);
                prune_map(local_path);
                send_path(local_path, feature_state, peer_mac);

                // If first to finish move out of the way
                if (!second_to_finish) move_fixed_forward(motor_phase_pins, &pcnt_unit, 800);
                
                // Sit still
                while(1) {
                    vTaskDelay(pdMS_TO_TICKS(500));
                }
            } else if (movement_state == TURNING_RIGHT) {
                turn_right(ir_pins, ir_values, motor_phase_pins, feature_state);
            } else if (movement_state == TURNING_LEFT) {
                turn_left(ir_pins, ir_values, motor_phase_pins, feature_state);
            } else if (movement_state == U_TURN) {
                u_turn(ir_pins, ir_values, motor_phase_pins, feature_state);
            }
        }
    }
}