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
        } else if (feature_state & LEFT_TURN) {
            *movement_state = TURNING_LEFT;
        } else if (feature_state & STRAIGHT_LINE) {
            *movement_state = GOING_STRAIGHT;
        } else if (feature_state & RIGHT_TURN) {
            *movement_state = TURNING_RIGHT;
        } else {
            *movement_state = GOING_STRAIGHT;
        }

        // if (feature_state & END_OF_MAZE) {
        //     *movement_state = STOPPED;
        // } else if (feature_state == DEAD_END) {
        //     *movement_state = U_TURN;
        // } else if (feature_state == STRAIGHT_LINE) {
        //     *movement_state = GOING_STRAIGHT;
        // } else if (feature_state & RIGHT_TURN) {
        //     *movement_state = TURNING_RIGHT;
        // } else if (feature_state & STRAIGHT_LINE) {
        //     *movement_state = GOING_STRAIGHT;
        // } else if (feature_state & LEFT_TURN) {
        //     *movement_state = TURNING_LEFT;
        // } else {
        //     *movement_state = GOING_STRAIGHT;
        // }

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

void prune_map(uint8_t *node_list) {
    static int actual_features = 0;

    /* Find number of actuals turns in the path */
    for (int i=0; i<NUM_MAP_FEATURES; i++) {
        if (node_list[i] == 0) break;
        actual_features++;
    }

    /* Loop through all turns in path */
    for (int i = 0; i < actual_features; i++) {

        /* Find First U-Turn */
        if (node_list[i] == U_TURN) {

            // Deal with case where first turn is a U-Turn
            if (i == 0) {
                if (node_list[i + 1] == U_TURN) {
                } else {
                    continue;
                }

            // Deal with other cases
            } else {

                uint16_t compare_nodes = 0;
                compare_nodes = (node_list[i-1] << 8) | node_list[i+1];

                switch (compare_nodes){
                    case RUS:
                    case SUR:
                        node_list[i-1] = TURNING_LEFT;
                        break;
                    case RUL:
                    case LUR:
                    case SUS:
                        node_list[i-1] = U_TURN;
                        break;
                    case RUR:
                    case LUL:
                        node_list[i-1] = GOING_STRAIGHT;
                        break;  
                    case LUS:
                    case SUL:
                        node_list[i-1] = TURNING_RIGHT;
                        break;
                }
            }

            /* Move remaining turns over to fill gap made by pruning, then recurse on the new list */
            if ((actual_features - i - 2) > 0) {

                // Move rest of path over to fill space made by turns removed
                memmove(&(node_list[i]), &(node_list[i+2]), sizeof(uint8_t) * (actual_features - i - 2));
                
                // Clear remaining turns that weren't overwritten when moved
                memset( &(node_list[actual_features - 2]) , 0, sizeof(uint8_t) * (NUM_MAP_FEATURES - actual_features - 2));
                
                prune_map(node_list);
                break;
            } else {
                break;
            }
        }
    }
}

void add_u_turns(uint8_t *node_list) {

    int actual_features = 0;

    /* Find number of actuals turns in the path */
    for (int i=0; i<NUM_MAP_FEATURES; i++) {
        if (node_list[i] == 0) break;
        actual_features++;
    }

    // Make room for first U-Turn
    memmove(node_list + 1, node_list, sizeof(uint8_t) * actual_features);
    
    // Set first and last as U turn
    node_list[0] = U_TURN;
    node_list[actual_features + 1] = U_TURN;

}


void backtrack(uint8_t* node_list) {

    for (int i = 0; i < NUM_MAP_FEATURES; i++) {
        if (node_list[i] == TURNING_LEFT) node_list[i] = TURNING_RIGHT;
        else if (node_list[i] == TURNING_RIGHT) node_list[i] = TURNING_LEFT;
    }

    int actual_features = 0;

    /* Find number of actuals turns in the path */
    for (int i=0; i<NUM_MAP_FEATURES; i++) {
        if (node_list[i] == 0) break;
        actual_features++;
    }

    int start = 0;
    int end = actual_features - 1;
    uint8_t temp;

    while (start < end) {
        temp = node_list[start];
        node_list[start] = node_list[end];
        node_list[end] = temp;

        start++;
        end--;
    }

}

void generate_shortest_path(uint8_t* local_path, uint8_t* solved_path, uint8_t* final_path) {
    
    add_u_turns(local_path);
    backtrack(local_path);

    int local_path_features = 0;
    int solved_path_features = 0;

    /* Find number of actuals turns in the path */
    // Local path
    for (int i=0; i<NUM_MAP_FEATURES; i++) {
        if (local_path[i] == 0) break;
        local_path_features++;
    }

    // solved path
    for (int i=0; i<NUM_MAP_FEATURES; i++) {
        if (solved_path[i] == 0) break;
        solved_path_features++;
    }

    // Combine local and solved path
    // memcpy(final_path, local_path, sizeof(uint8_t) * local_path_features);
    memcpy(local_path + local_path_features, solved_path, sizeof(uint8_t) * solved_path_features);

    // Prune final map
    prune_map(local_path);
}