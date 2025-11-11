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