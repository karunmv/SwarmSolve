#include "main.h"
#include "esp_now_comms.h"

const char* TAG_RX = "ESP-NOW RX";
static const char* TAG_TX = "ESP-NOW TX";
uint8_t broadcast_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t esp_mac[6];

void esp_now_recv_callback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
    
    if (data[0] == PATH_PACKET) {
        xQueueSend(path_transfer_queue, data, ESPNOW_MAX_DELAY);
    } else if (data[0] == STARTED_PACKET) {
        uint8_t from_button = 0;
        xQueueSend(button_press_queue, &from_button, ESPNOW_MAX_DELAY);
    }
}

void esp_now_send_callback(const esp_now_send_info_t *tx_info, esp_now_send_status_t status) {
    ESP_LOGI(TAG_RX,"tx cb");
}

void send_message(void) {
        char *message = "Hi Aedan, from Karun :)";
        esp_err_t err = esp_now_send(broadcast_mac, (uint8_t *)message, strlen(message));
        
        ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
}

void send_state(uint8_t feature_state, uint8_t movement_state) {
    uint8_t states[2] = {feature_state, movement_state};

    esp_err_t err = esp_now_send(broadcast_mac, states, sizeof(uint8_t) * 2);
    ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
}

void send_path(uint8_t *path, uint8_t feature_state) {

    uint8_t path_tx[NUM_MAP_FEATURES + 1] = {0};
    path_tx[0] = PATH_PACKET;
    memcpy(&(path_tx[1]), path, NUM_MAP_FEATURES);

    /* Only send if at a node */
    if ((feature_state & STRAIGHT_CHECKED) || (feature_state & END_OF_MAZE) || (feature_state & DEAD_END)) {
        esp_err_t err = esp_now_send(broadcast_mac, path_tx, sizeof(uint8_t) * (NUM_MAP_FEATURES + 1));
        ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
    }
}

void send_started(void) {
    uint8_t tx_packet = STARTED_PACKET;

    esp_err_t err = esp_now_send(broadcast_mac, &tx_packet, sizeof(uint8_t));
    ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));

}

void send_ir_values(uint32_t *ir_values) {

    uint8_t all_values[8]={0};

    for(int i=0; i<8; i++){
        uint32_t value = ir_values[i];

        uint8_t cast_values[4] = {(uint8_t)(value & 0xff), (uint8_t)((value >> 8) & 0xff), (uint8_t)((value >> 16) & 0xff), (uint8_t)((value >> 24) & 0xff)};

        all_values[i] = *cast_values;
        // esp_err_t err = esp_now_send(broadcast_mac, cast_values, sizeof(uint8_t*)*4);
        // ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
    }
    esp_err_t err = esp_now_send(broadcast_mac, all_values, sizeof(uint32_t *) * 8 * 8);
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