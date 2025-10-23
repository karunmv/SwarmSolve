#include "main.h"
#include "navigation_control.h"


void update_movement_state(uint8_t feature_state, uint8_t *robot_state) {

    /* Check if turn exists, and if so double check if the line continues straight */
    if ((feature_state & RIGHT_TURN) || (feature_state & LEFT_TURN)) {
        
        if ((feature_state & STRAIGHT_CHECKED) == 0) {
            *robot_state = CHECKING_STRAIGHT;
            return;
        } else {
            *robot_state = STOPPED;
        }

    } 

    if (feature_state == DEAD_END) {
        *robot_state = STOPPED;
    } else if (feature_state == STRAIGHT_LINE) {
        *robot_state = GOING_STRAIGHT;
    }

}