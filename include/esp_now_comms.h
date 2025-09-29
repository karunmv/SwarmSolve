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


typedef struct {
    uint16_t idk;
} node_data_t;

typedef struct {
    uint8_t mac_addr[6];
    uint8_t *node_data;
} swarm_node_t;


void esp_now_recv_callback(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len);

void esp_now_send_callback(const esp_now_send_info_t * tx_info, esp_now_send_status_t status);

void esp_broadcast_setup(void);

void send_message(void);

void wifi_sta_init(void);

#endif