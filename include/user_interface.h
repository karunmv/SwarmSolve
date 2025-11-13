#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#define BUTTON_PIN 9

#define ESP_INTR_FLAG_DEFAULT 0

void init_user_button(int button_gpio);

#endif