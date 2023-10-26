#ifndef	BLE_PWR_MGMT_DEFINITION_H_
#define BLE_PWR_MGMT_DEFINITION_H_

#include "nrf_pwr_mgmt.h"

void sleep_mode_enter(void);
void log_init(void);
void idle_state_handle(void);
bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event);
void power_management_init(void);
bool get_system_status(void);
void change_system_status(bool system_status);
bool get_proximity_status(void);
void change_proximity_status(bool system_status);
bool get_system_charging_status(void);
void system_state_handler(void);
void provision_state_handler(uint8_t new_provision_state);
void reset_source_handler(void);
void change_brightness_level(uint16_t brightness_level);
#endif 
