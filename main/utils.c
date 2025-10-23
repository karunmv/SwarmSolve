#include "main.h"
#include "QTRX-MD-NRC.h"


void print_IR_values(uint32_t *ir_values) {

    printf("IR_ARRAY Values: [ ");
    for (int i = 0; i < IR_PIN_COUNT; i++) {
        printf("%ld ", ir_values[i]);
        
    }
    printf("]\n");

}

void print_feature_state(uint8_t feature_state) {
    printf("Features\n");
    if (feature_state & STRAIGHT_LINE) printf("Straight_Line ");
    if (feature_state & RIGHT_TURN) printf("Right_Turn ");
    if (feature_state & LEFT_TURN) printf("LEFT_TURN ");
    if (feature_state & DEAD_END) printf("DEAD_END ");
    if (feature_state & LOST_LINE) printf("LOST_LINE ");
    if (feature_state & END_OF_MAZE) printf("END_OF_MAZE ");
    if (feature_state & STRAIGHT_CHECKED) printf("STRAIGHT_CHECKED ");
    printf("\n");
}

void print_movement_state(uint8_t movement_state) {
    printf("Movement\n");
    if (movement_state & GOING_STRAIGHT) printf("GOING_STRAIGHT ");
    if (movement_state & TURNING_LEFT) printf("TURNING_LEFT ");
    if (movement_state & TURNING_RIGHT) printf("TURNING_RIGHT ");
    if (movement_state & U_TURN) printf("U_TURN ");
    if (movement_state & REVERSING) printf("REVERSING ");
    if (movement_state & STOPPED) printf("STOPPED ");
    if (movement_state & CHECKING_STRAIGHT) printf("CHECKING_STRAIGHT ");
    printf("\n");
}