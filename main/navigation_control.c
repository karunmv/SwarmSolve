#include "main.h"
#include "navigation_control.h"
#include "feature_detection.h"


void update_movement_state(uint8_t feature_state, uint8_t *robot_state) {

    if (feature_state == DEAD_END) {
        *robot_state = TURNING_RIGHT;
    } else if (feature_state == STRAIGHT_LINE) {
        *robot_state = GOING_STRAIGHT;
    } else if (feature_state & RIGHT_TURN) {
        *robot_state = TURNING_RIGHT;
    } else {
        *robot_state = STOPPED;
    }

}