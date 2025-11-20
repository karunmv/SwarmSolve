#ifndef USER_INTERFACE_H
#define USER_INTERFACE_H

#define BUTTON_PIN 9
#define ESP_INTR_FLAG_DEFAULT 0

/**
 * @brief Initializes a user button that will trigger an interrupt when pressed
 * @param button_gpio GPIO pin number of the button
 */
void init_user_button(int button_gpio);

#endif