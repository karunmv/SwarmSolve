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

// Threshold value for the line
#define PRIMARY_IR_THRESHOLD    1200

// Exploration Strategy, ie always left, always right, always straight etc...
#define ALWAYS_RIGHT            0
#define ALWAYS_LEFT             1
#define EXPLORATION_STRATEGY    ALWAYS_LEFT

// Peer mac
#define BLACK_BOT_MAC       {0x58, 0x8C, 0x81, 0x41, 0x3A, 0x8C}
#define LAVENDER_BOT_MAC    {0x58, 0x8C, 0x81, 0x40, 0xBD, 0x4C}
#define BROADCAST_MAC       {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}
#define PEER_MAC            LAVENDER_BOT_MAC

// Motor Movement
#define BASE_FORWARD_SPEED 25
#define BASE_TURN_SPEED 20
#define FORWARD_TUNE 0 // Positive = Left, negative = Right
#define CHECK_STRAIGHT_PULSES 600

// Important Defines
#define NUM_MAP_FEATURES 100
#define NUM_MOTORS 2
#define DEFAULT_QUEUE_SIZE 5

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
#define TURNING_RIGHT       (1 << 1)
#define TURNING_LEFT        (1 << 2)
#define U_TURN              (1 << 3)
#define REVERSING           (1 << 4)
#define STOPPED             (1 << 5)
#define CHECKING_STRAIGHT   (1 << 6)

// Prune defines
#define RUS (TURNING_RIGHT << 8) | GOING_STRAIGHT
#define RUL (TURNING_RIGHT << 8) | TURNING_LEFT
#define RUR (TURNING_RIGHT << 8) | TURNING_RIGHT
#define RUU (TURNING_RIGHT << 8) | U_TURN
#define LUS (TURNING_LEFT << 8) | GOING_STRAIGHT
#define LUL (TURNING_LEFT << 8) | TURNING_LEFT
#define LUR (TURNING_LEFT << 8) | TURNING_RIGHT
#define LUU (TURNING_LEFT << 8) | U_TURN
#define SUS (GOING_STRAIGHT << 8) | GOING_STRAIGHT
#define SUL (GOING_STRAIGHT << 8) | TURNING_LEFT
#define SUR (GOING_STRAIGHT << 8) | TURNING_RIGHT
#define SUU (GOING_STRAIGHT << 8) | U_TURN

/* Global Variables */
extern QueueHandle_t path_transfer_queue;
extern QueueHandle_t button_press_queue;

#endif