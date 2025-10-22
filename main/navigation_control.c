#include "main.h"
#include "navigation_control.h"


void update_movement_state(uint8_t feature_state, uint8_t *robot_state) {

    if (feature_state == DEAD_END) {
        *robot_state = STOPPED;
    } else {
        *robot_state = GOING_STRAIGHT;
    }
}