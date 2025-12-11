#include "main.h"
#include "esp_now_comms.h"
#include "QTRX-MD-NRC.h"

const char* TAG_RX = "ESP-NOW RX";
static const char* TAG_TX = "ESP-NOW TX";
uint8_t broadcast_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
uint8_t esp_mac[6];

void esp_now_recv_callback(const esp_now_recv_info_t *esp_now_info, const uint8_t *data, int data_len) {
    
    // Check packet type then queue data based on message type
    if (data[0] == PATH_PACKET) {
        xQueueSend(path_transfer_queue, data, ESPNOW_MAX_DELAY);
    } else if (data[0] == STARTED_PACKET) {
        uint8_t from_button = 0; // Denotes that the start came from the other robot
        xQueueSend(button_press_queue, &from_button, ESPNOW_MAX_DELAY);
    }
}

void esp_now_send_callback(const esp_now_send_info_t *tx_info, esp_now_send_status_t status) {
    // ESP_LOGI(TAG_RX,"tx cb");
}

void send_state(uint8_t feature_state, uint8_t movement_state, uint8_t *peer_mac) {
    
    // State packet to send
    uint8_t states[3] = {STATE_PACKET, feature_state, movement_state};

    // Send message
    esp_err_t err = esp_now_send(peer_mac, states, sizeof(uint8_t) * 3);
    ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
}

void send_path(uint8_t *path, uint8_t feature_state, uint8_t *peer_mac) {

    // Inserts packet label to first byte
    uint8_t path_tx[NUM_MAP_FEATURES + 1] = {0};
    path_tx[0] = PATH_PACKET;
    memcpy(&(path_tx[1]), path, NUM_MAP_FEATURES);

    /* Only send if at a node */
    if ((feature_state & STRAIGHT_CHECKED) || (feature_state & END_OF_MAZE) || (feature_state & DEAD_END)) {
        
        // Send message
        esp_err_t err = esp_now_send(peer_mac, path_tx, sizeof(uint8_t) * (NUM_MAP_FEATURES + 1));
        ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
    }
}

void send_started(uint8_t *peer_mac) {
    uint8_t tx_packet = STARTED_PACKET;

    // Send message
    esp_err_t err = esp_now_send(peer_mac, &tx_packet, sizeof(uint8_t));
    ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));

}

void send_ir_values(uint32_t *ir_values, uint8_t *peer_mac) {

    uint8_t ir_tx[(IR_PIN_COUNT * 4) + 1];
    uint8_t *ir_bytes;

    // Makes first byte packet type
    ir_tx[0] = IR_PACKET;
    ir_bytes = &(ir_tx[1]);

    // Maps uint32_t to four uint8_t for tx
    for (int i = 0; i < IR_PIN_COUNT; i++) {
        
        ir_bytes[i * 4] = (uint8_t)((ir_values[i] >> (0 * 8)) & 0xFF);
        ir_bytes[(i * 4) + 1] = (uint8_t)((ir_values[i] >> (1 * 8)) & 0xFF);
        ir_bytes[(i * 4) + 2] = (uint8_t)((ir_values[i] >> (2 * 8)) & 0xFF);
        ir_bytes[(i * 4) + 3] = (uint8_t)((ir_values[i] >> (3 * 8)) & 0xFF);
    }

    // Send message
    esp_err_t err = esp_now_send(peer_mac, ir_tx, sizeof(ir_tx));
    ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
}

void esp_setup(uint8_t *peer_mac, uint8_t *local_mac) {
    // Print mac address of current board
    ESP_ERROR_CHECK( esp_read_mac(local_mac, ESP_MAC_WIFI_STA) );
    ESP_LOGI(TAG_RX, "Mac address: " MACSTR "", local_mac[0], local_mac[1], local_mac[2], local_mac[3], local_mac[4], local_mac[5]);

    // Init espnow and set callback functions
    ESP_ERROR_CHECK( esp_now_init() );
    ESP_ERROR_CHECK( esp_now_register_send_cb(esp_now_send_callback) );
    ESP_ERROR_CHECK( esp_now_register_recv_cb(esp_now_recv_callback) );

    // Add esp_now peer mac address
    esp_now_peer_info_t peer_info = {0};
    peer_info.channel = 1; 
    peer_info.encrypt = false;
    memcpy(peer_info.peer_addr, peer_mac, 6);
    ESP_ERROR_CHECK( esp_now_add_peer(&peer_info) );
}

void wifi_sta_init(void) {

    // Initialize nvs flash memory
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        nvs_flash_erase();
        ret = nvs_flash_init();
    }

    // Create event loop
    ESP_ERROR_CHECK( esp_netif_init() );
    ESP_ERROR_CHECK( esp_event_loop_create_default() );

    // Configure wifi
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_ps(WIFI_PS_NONE) );
    ESP_ERROR_CHECK( esp_wifi_start() );
    ESP_ERROR_CHECK( esp_wifi_set_channel(1, WIFI_SECOND_CHAN_NONE) );
}

void send_message(void) {
        char *message = "Hi Aedan, from Karun :)";
        esp_err_t err = esp_now_send(broadcast_mac, (uint8_t *)message, strlen(message));
        ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
}