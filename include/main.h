#ifndef MAIN_H
#define MAIN_H

// Includes
#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "string.h"
#include "driver/gpio.h"


// Important Defines
#define NUM_MOTORS 2

// Feature state defines
#define END_OF_MAZE     (0)
#define STRAIGHT_LINE   (1 << 0)
#define RIGHT_TURN      (1 << 1)
#define LEFT_TURN       (1 << 2)
#define DEAD_END        (1 << 3)
#define GOING_STRAIGHT  (1 << 4)
#define TURNING_LEFT    (1 << 5)
#define TURNING_RIGHT   (1 << 6)
#define U_TURN          (1 << 7)

#endif