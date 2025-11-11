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

void prune_map(uint8_t *node_list){
    uint8_t temp_map[NUM_MAP_FEATURES] = {0}; // All elements initialized to 0
    static int actual_features = 0;
    static int temp_index = 0;
    int u_flag = 0;

    for(int i=0; i<NUM_MAP_FEATURES; i++){
        if (node_list[i] == 0) break;
        actual_features++;
    }

    for(int j=0; j<actual_features; j++){
        if((node_list[j] == U_TURN) && (node_list[j-1] == U_TURN) && (j != 0)) {
            temp_index--;
            continue;
        }
        if((node_list[j] == U_TURN) && (j != actual_features - 1) && (j != 0)){
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
                    u_flag = 1;
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

    if(u_flag){
        prune_map(temp_map);
        u_flag = 0;
    }

    memcpy(temp_map, node_list, sizeof(temp_map));
}

void add_u_turns(uint8_t *pruned_list){
    uint8_t temp_map[NUM_MAP_FEATURES] = {0}; // All elements initialized to 0

    temp_map[0] = U_TURN;

    for(int i=0; i<NUM_MAP_FEATURES; i++){
        temp_map[i+1] = pruned_list[i];
        if (pruned_list[i] == 0){
            temp_map[i] = U_TURN;
            break;
        }
    }
    memcpy(pruned_list, temp_map, sizeof(temp_map));
}

void backtrack(uint8_t* pruned_list){
    uint8_t temp_map[NUM_MAP_FEATURES] = {0}; // All elements initialized to 0
    static int actual_features = 0;
    static int bt_index = 0;

    for(int i=0; i<NUM_MAP_FEATURES; i++){
        if (pruned_list[i] == 0) break;
        actual_features++;
    }

    for(int j=0; j<actual_features; j++){
        if(pruned_list[j] == TURNING_LEFT){
            temp_map[bt_index] = TURNING_RIGHT;
            bt_index++;
        } else if(pruned_list[j] == TURNING_RIGHT){
            temp_map[bt_index] = TURNING_LEFT;
            bt_index++;
        } else{
            temp_map[bt_index] = pruned_list[j];
            bt_index++;
        }
    }
    memcpy(pruned_list, temp_map, sizeof(temp_map));
}

void generate_shortest_path(uint8_t* node_list_R1, uint8_t* node_list_R2, uint8_t* final_map){
    static int final_index = 0;
    
    prune_map(node_list_R1);
    prune_map(node_list_R2);
    backtrack(node_list_R2);
    add_u_turns(node_list_R2);
    prune_map(node_list_R2);
    
    for(int i=0; i<NUM_MAP_FEATURES; i++){
        if (node_list_R2[i] == 0) break; 
        final_map[final_index] = node_list_R2[i];
        final_index++;
    }

    for(int j=0; j<NUM_MAP_FEATURES; j++){
        if (node_list_R1[j] == 0) break; 
        final_map[final_index] = node_list_R1[j];
        final_index++;
    }

    prune_map(final_map);
}