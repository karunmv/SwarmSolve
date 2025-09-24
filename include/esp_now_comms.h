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

void esp_now_recv_callback(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len);

void wifi_sta_init(void);

#endif