#include "main.h"
#include "navigation_control.h"
#include "feature_detection.h"


void update_movement_state(uint8_t feature_state, uint8_t *robot_state) {

    if (feature_state & END_OF_MAZE) {
        *robot_state = STOPPED;
    } else if (feature_state == DEAD_END) {
        *robot_state = TURNING_RIGHT;
    } else if (feature_state == STRAIGHT_LINE) {
        *robot_state = GOING_STRAIGHT;
    } else if (feature_state & RIGHT_TURN) {
        *robot_state = TURNING_RIGHT;
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
    uint8_t temp_map[NUM_MAP_FEATURES] = (uint8_t *)calloc(NUM_MAP_FEATURES, sizeof(uint8_t));
    static int actual_features = 0;
    static int temp_index = 0;
 
    for(int i=0; i<NUM_MAP_FEATURES; i++){
        if (node_list[i] == 0) break;
        actual_features++;
    }

    for(int j=0; j<actual_features-1; j++){
        uint16_t compare_nodes = 0;
        compare_nodes = (compare_nodes | (node_list[j] << 8)) | node_list[j+1];

        switch (compare_nodes){
            case RU:
                temp_map[temp_index] = TURNING_LEFT;
                temp_index++;
                break;
            case LU:
                temp_map[temp_index] = TURNING_RIGHT;
                temp_index++;
                break;
            case SU:
                temp_map[temp_index] = U_TURN;
                temp_index++;
                break;
            case SS:
                temp_map[temp_index] = GOING_STRAIGHT;
                temp_index++;
                temp_map[temp_index] = GOING_STRAIGHT;
                temp_index++;
                break;
            case RR:
                temp_map[temp_index] = TURNING_RIGHT;
                temp_index++;
                temp_map[temp_index] = TURNING_RIGHT;
                temp_index++;
                break;
            case LL:
                temp_map[temp_index] = TURNING_LEFT;
                temp_index++;
                temp_map[temp_index] = TURNING_LEFT;
                temp_index++;
                break;
            case RE:
                temp_map[temp_index] = TURNING_RIGHT;
                temp_index++;
                temp_map[temp_index] = STOPPED;
                temp_index++;
                break;
            case LE:
                temp_map[temp_index] = TURNING_LEFT;
                temp_index++;
                temp_map[temp_index] = STOPPED;
                temp_index++;
                break;
            case SE:
                temp_map[temp_index] = GOING_STRAIGHT;
                temp_index++;
                temp_map[temp_index] = STOPPED;
                temp_index++;
                break;
        }
    }
    return temp_map;

}