#include "main.h"
#include "QTRX-MD-NRC.h"


void print_IR_values(uint32_t *ir_values) {

    printf("IR_ARRAY Values: [ ");
    for (int i = 0; i < IR_PIN_COUNT; i++) {
        printf("%ld ", ir_values[i]);
        
    }
    printf("]\n");

}