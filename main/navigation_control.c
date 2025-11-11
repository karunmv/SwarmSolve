#include "main.h"
#include "navigation_control.h"
#include "feature_detection.h"


void update_movement_state(uint8_t feature_state, uint8_t *movement_state, uint8_t *path, uint8_t following_path) {

    uint8_t at_node = (feature_state & STRAIGHT_CHECKED) || (feature_state & U_TURN) || (feature_state & END_OF_MAZE);

    /* Robot is currently following a list of turns */
    if (following_path) {

        if (at_node) {
            static int turn_index = 0;

            *movement_state = path[turn_index];

            turn_index++;
        } else {
            *movement_state = GOING_STRAIGHT;
        }

    /* Robot is currently exploring the maze and recording it's path */
    } else {
        if (feature_state & END_OF_MAZE) {
            *movement_state = STOPPED;
        } else if (feature_state == DEAD_END) {
            *movement_state = U_TURN;
        } else if (feature_state == STRAIGHT_LINE) {
            *movement_state = GOING_STRAIGHT;
        } else if (feature_state & RIGHT_TURN) {
            *movement_state = TURNING_RIGHT;
        } else if (feature_state & STRAIGHT_LINE) {
            *movement_state = GOING_STRAIGHT;
        } else if (feature_state & LEFT_TURN) {
            *movement_state = TURNING_LEFT;
        } else {
            *movement_state = GOING_STRAIGHT;
        }

        update_path(feature_state, movement_state, path);
    }

}

void update_path(uint8_t feature_state, uint8_t *movement_state, uint8_t *path) {
    static uint8_t turn_index = 0;

    if((feature_state & STRAIGHT_CHECKED)){
        path[turn_index] = *movement_state;
        turn_index++;
    } else if (*movement_state == U_TURN){
        path[turn_index] = U_TURN;
        turn_index++;
    } else if (*movement_state == STOPPED){
        path[turn_index] = STOPPED;
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