#include "esp_now_comms.h"

const char* TAG_RX = "ESP-NOW RX";
static const char* TAG_TX = "ESP-NOW TX";
uint8_t esp_mac[6];

void esp_now_recv_callback(const esp_now_recv_info_t * esp_now_info, const uint8_t *data, int data_len)
{
    ESP_LOGI(TAG_TX,"received data : %.*s", data_len, data);
}

void esp_now_send_callback(const esp_now_send_info_t * tx_info, esp_now_send_status_t status)
{
    ESP_LOGI(TAG_RX,"tx cb");
}

void esp_now_add_to_swarm(void)
{
    esp_now_peer_info_t peer_info = {0};
    peer_info.channel = 1; 
    peer_info.encrypt = false;
    //uint8_t esp_mac[6] = {0x40,0x4c,0xca,0x44,0xdf,0x44};
    uint8_t esp_mac[6] = {0xff,0xff,0xff,0xff,0xff,0xff};
    memcpy(peer_info.peer_addr, esp_mac, 6);
    esp_now_add_peer(&peer_info);
}

void send_message(void)
{
        char* message = "Hi Aedan, from Karun :)";
        esp_err_t err = esp_now_send(esp_mac, (uint8_t *) message, strlen(message));
        
        ESP_LOGI(TAG_TX,"esp now status : %s", esp_err_to_name(err));
}

void wifi_sta_init(void)
{
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

    // Sender MAC
    uint8_t my_esp_mac[6] = {0x40,0x4c,0xca,0x44,0xf6,0x7c};
    esp_read_mac(my_esp_mac, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG_TX, "peer mac " MACSTR "", my_esp_mac[0], my_esp_mac[1], my_esp_mac[2], my_esp_mac[3], my_esp_mac[4], my_esp_mac[5]);

    // Receiver MAC
    esp_read_mac(esp_mac, ESP_MAC_WIFI_STA);
    ESP_LOGI(TAG_RX, "peer mac " MACSTR "", esp_mac[0], esp_mac[1], esp_mac[2], esp_mac[3], esp_mac[4], esp_mac[5]);
}