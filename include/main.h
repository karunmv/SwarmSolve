#ifndef MAIN_H
#define MAIN_H

// Includes
#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "string.h"

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

#endif