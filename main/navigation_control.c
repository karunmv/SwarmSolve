#include "main.h"
#include "navigation_control.h"
#include "feature_detection.h"


void update_movement_state(uint8_t feature_state, uint8_t *robot_state) {

    if (feature_state & END_OF_MAZE) {
        *robot_state = STOPPED;
    } else if (feature_state == DEAD_END) {
        *robot_state = U_TURN;
    } else if (feature_state == STRAIGHT_LINE) {
        *robot_state = GOING_STRAIGHT;
    } else if (feature_state & RIGHT_TURN) {
        *robot_state = TURNING_RIGHT;
    } else if (feature_state & LEFT_TURN) {
        *robot_state = TURNING_LEFT;
    } else {
        *robot_state = GOING_STRAIGHT;
    }

}

void occupancy_grid(uint8_t feature_state, uint8_t *robot_state, uint8_t *node_list){
    static uint8_t node_index = 0;

    if((feature_state & STRAIGHT_CHECKED)){
        node_list[node_index] = *robot_state;
        node_index++;
    } else if (*robot_state == U_TURN){
        node_list[node_index] = U_TURN;
        node_index++;
    } else if (*robot_state == STOPPED){
        node_list[node_index] = STOPPED;
    }
}

uint8_t* prune_map(uint8_t *node_list){
    uint8_t temp_map[NUM_MAP_FEATURES] = {0}; // All elements initialized to 0
    static int actual_features = 0;
    static int temp_index = 0;

    for(int i=0; i<NUM_MAP_FEATURES; i++){
        if (node_list[i] == 0) break;
        actual_features++;
    }

    for(int j=0; j<actual_features; j++){
        if(node_list[j] == U_TURN){
            uint16_t compare_nodes = 0;
            compare_nodes = (node_list[j-1] << 8) | node_list[j+1];
            switch (compare_nodes){
                case RUS:
                case SUR:
                    temp_map[temp_index-1] = TURNING_LEFT;
                    temp_index++;
                    break;
                case RUL:
                case LUR:
                case SUS:
                    temp_map[temp_index-1] = U_TURN;
                    temp_index++;
                    break;
                case RUR:
                case LUL:
                    temp_map[temp_index-1] = GOING_STRAIGHT;
                    temp_index++;
                    break;  
                case LUS:
                case SUL:
                    temp_map[temp_index-1] = TURNING_RIGHT;
                    temp_index++;
                    break;
            }
        } else if(node_list[j-1] == U_TURN){
            continue;
        } else {
            temp_map[temp_index] = node_list[j];
            temp_index++;
        }
    }
    return temp_map;

}