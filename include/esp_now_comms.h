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

typedef struct {
    uint16_t idk;
} node_data_t;

typedef struct {
    uint8_t mac_addr[6];
    uint8_t *node_data;
} swarm_node_t;


void esp_now_recv_callback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len);

void esp_now_send_callback(const esp_now_send_info_t *tx_info, esp_now_send_status_t status);

void esp_broadcast_setup(void);

void esp_setup(uint8_t *peer_mac, uint8_t *local_mac);

// void send_state(uint8_t feature_state, uint8_t movement_state);
void send_state(uint8_t feature_state, uint8_t movement_state, uint8_t *peer_mac);

// void send_path(uint8_t *path, uint8_t feature_state);
void send_path(uint8_t *path, uint8_t feature_state, uint8_t *peer_mac);

// void send_started(void);
void send_started(uint8_t *peer_mac);

// void send_ir_values(uint32_t *ir_values);
void send_ir_values(uint32_t *ir_values, uint8_t *peer_mac);

void wifi_sta_init(void);

void send_message(void);

#endif