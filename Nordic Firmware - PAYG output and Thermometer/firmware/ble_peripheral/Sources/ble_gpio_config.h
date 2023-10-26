#ifndef BLE_GPIO_CONFIG_H_
#define BLE_GPIO_CONFIG_H_

#include <stdbool.h>
#include "bsp_btn_ble.h"
//#include "bsp.h"
void gpio_init(void);
void buttons_leds_init(bool * p_erase_bonds);
void leds_init(void);
void bsp_event_handler(bsp_event_t event);
uint32_t get_temperature(void);
void GPIO_SENSE_RX(bool cmd);
//static void button_event_handler(uint8_t pin_no, uint8_t button_action);
//static void MULTIPURPOSE_KEY_timeout_handler(void * p_context);
#endif 
