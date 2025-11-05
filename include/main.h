#ifndef MAIN_H
#define MAIN_H

// Includes
#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "string.h"
#include "driver/gpio.h"


#define PRIMARY_IR_THRESHOLD    1300
#define BASE_FORWARD_SPEED 40
#define BASE_TURN_SPEED 20
#define NUM_MAP_FEATURES 10

// Important Defines
#define NUM_MOTORS 2

// Feature state defines
#define STRAIGHT_LINE       (1 << 0)
#define RIGHT_TURN          (1 << 1)
#define LEFT_TURN           (1 << 2)
#define DEAD_END            (1 << 3)
#define LOST_LINE           (1 << 4)
#define END_OF_MAZE         (1 << 5)
#define STRAIGHT_CHECKED    (1 << 6)

// Robot state defines
#define GOING_STRAIGHT      (1 << 0)
#define TURNING_LEFT        (1 << 1)
#define TURNING_RIGHT       (1 << 2)
#define U_TURN              (1 << 3)
#define REVERSING           (1 << 4)
#define STOPPED             (1 << 5)
#define CHECKING_STRAIGHT   (1 << 6)

// Prune defines
#define RU (((1 << 2) << 8) | (1 << 3))
#define LU (((1 << 1) << 8) | (1 << 3))
#define SU (((1 << 0) << 8) | (1 << 3))
#define SS (((1 << 0) << 8) | (1 << 0))
#define RR (((1 << 2) << 8) | (1 << 2))
#define LL (((1 << 1) << 8) | (1 << 1))
#define RE (((1 << 2) << 8) | (1 << 5))
#define LE (((1 << 1) << 8) | (1 << 5))
#define SE (((1 << 0) << 8) | (1 << 5))

#endif