#ifndef MAIN_H
#define MAIN_H

// Includes
#include <stdio.h>
#include <stdint.h>
#include "esp_log.h"
#include "string.h"
#include "driver/gpio.h"
#include "driver/pulse_cnt.h"
#include "freertos/FreeRTOS.h"


#define PRIMARY_IR_THRESHOLD    1250

// Exploration Strategy, ie always left, always right, always straight etc...


// Motor Movement
#define BASE_FORWARD_SPEED 55
#define BASE_TURN_SPEED 30
#define NUM_MAP_FEATURES 20
#define CHECK_STRAIGHT_PULSES 620

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
#define RUS ((1 << 1) << 8) | (1 << 0)
#define RUL ((1 << 1) << 8) | (1 << 1)
#define RUR ((1 << 1) << 8) | (1 << 2)
#define RUU ((1 << 1) << 8) | (1 << 3)
#define LUS ((1 << 2) << 8) | (1 << 0)
#define LUL ((1 << 2) << 8) | (1 << 1)
#define LUR ((1 << 2) << 8) | (1 << 2)
#define LUU ((1 << 2) << 8) | (1 << 3)
#define SUS ((1 << 0) << 8) | (1 << 0)
#define SUL ((1 << 0) << 8) | (1 << 1)
#define SUR ((1 << 0) << 8) | (1 << 2)
#define SUU ((1 << 0) << 8) | (1 << 3)

/* Global Variables */
extern QueueHandle_t path_transfer_queue;

#endif