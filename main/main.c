#include "main.h"
#include "esp_now_comms.h"


void app_main(void)
{
    wifi_sta_init();
    esp_now_init();
    esp_now_register_recv_cb(esp_now_recv_callback);
  
    while(1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
