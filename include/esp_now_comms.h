#ifndef ESP_NOW_COMMS_H
#define ESP_NOW_COMMS_H

#include <stdio.h>
#include "esp_now.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "string.h"
#include "nvs_flash.h"
#include "esp_now.h"

/* Defines */
#define PATH_PACKET         1
#define STARTED_PACKET      2
#define IR_PACKET           3
#define STATE_PACKET        4
#define ESPNOW_MAX_DELAY    512


/**
 * @brief Callback function called when a message is received
 * @param esp_now_info Info about the tx device
 * @param data Poiner to uint8_t array that contains the data
 * @param data_len Number of elements in data array
 */
void esp_now_recv_callback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len);

/**
 * @brief Callback function called when a message is received
 * @param tx_info Info about tx device
 * @param status espnow status
 */
void esp_now_send_callback(const esp_now_send_info_t *tx_info, esp_now_send_status_t status);

/**
 * @brief Setup espnow peer device
 * @param peer_mac Pointer to array that stores the mac address of the peer to be added
 * @param local_mac Pointer to array to store the local mac address of this robot
 */
void esp_setup(uint8_t *peer_mac, uint8_t *local_mac);

/**
 * @brief Sends the movement and feature state of the robot
 * @param feature_state the feature state of the robot
 * @param feature_state the movement state of the robot
 * @param peer_mac Pointer to lenth 6 array that contains the mac address of the peer to send the message to
 */
void send_state(uint8_t feature_state, uint8_t movement_state, uint8_t *peer_mac);

/**
 * @brief Sends the path the robot has explored to the other robot
 * @param path Pointer to the path of at most NUM_MAP_FEATURES to send to the peer
 * @param feature_state the movement state of the robot
 * @param peer_mac Pointer to lenth 6 array that contains the mac address of the peer to send the message to
 */
void send_path(uint8_t *path, uint8_t feature_state, uint8_t *peer_mac);

/**
 * @brief Send a message to the other robot that the started button has been pressed and it needs to start exploring the maze
 * @param peer_mac Pointer to lenth 6 array that contains the mac address of the peer to send the message to
 */
void send_started(uint8_t *peer_mac);

/**
 * @brief Sends the array of IR values to a peer
 * @param ir_values Array of IR values to be sent
 * @param peer_mac Pointer to lenth 6 array that contains the mac address of the peer to send the message to
 */
void send_ir_values(uint32_t *ir_values, uint8_t *peer_mac);

/**
 * @brief Initializes wifi as a station
 */
void wifi_sta_init(void);

/**
 * @brief Sends an example message, for debugging
 */
void send_message(void);

#endif