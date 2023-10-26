#ifndef BLE_TIMERS_DEFINITIONS_H_
#define BLE_TIMERS_DEFINITIONS_H_

#include "nrf.h"
#include "nrf_gpio.h"
#include "nrf_drv_rtc.h"
#include "nrf_drv_clock.h"
#include "app_error.h"
#include "ble_gpio_config.h" 
#include "ssl_config.h" 


void application_timers_start(void);
void timers_init(void);
void rtc_handler(nrf_drv_rtc_int_type_t int_type);
void rtc_config(void);

#endif 
