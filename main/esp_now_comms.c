#include "esp_now_comms.h"
#include "utils.h"

const char* TAG_RX = "ESP-NOW RX";
static const char* TAG_TX = "ESP-NOW TX";
uint8_t broadcast_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t esp_mac[6];

void esp_now_recv_callback(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len) {
    static int count = 0;

    printf("Received Data %d\n", count);
    print_feature_state(data[0]);
    print_movement_state(data[1]);

    count++;
}

void esp_now_send_callback(const esp_now_send_info_t * tx_info, esp_now_send_status_t status) {
    ESP_LOGI(TAG_RX,"tx cb");
}

void send_message(void) {
        char* message = "Hi Aedan, from Karun :)";
        esp_err_t err = esp_now_send(broadcast_mac, (uint8_t *)message, strlen(message));
        
        ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
}

void esp_broadcast_setup(void) {
    // Print mac address of current board
    esp_read_mac(esp_mac, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG_RX, "Mac address: " MACSTR "", esp_mac[0], esp_mac[1], esp_mac[2], esp_mac[3], esp_mac[4], esp_mac[5]);

    esp_now_init();
    esp_now_register_send_cb(esp_now_send_callback);
    esp_now_register_recv_cb(esp_now_recv_callback);

    // Add esp_now peer as broadcast address
    esp_now_peer_info_t peer_info = {0};
    peer_info.channel = 1; 
    peer_info.encrypt = false;
    memcpy(peer_info.peer_addr, broadcast_mac, 6);
    esp_now_add_peer(&peer_info);
}

void wifi_sta_init(void) {
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }

    esp_netif_init();
    esp_event_loop_create_default();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_ps(WIFI_PS_NONE);
    esp_wifi_start();
    esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE);
}