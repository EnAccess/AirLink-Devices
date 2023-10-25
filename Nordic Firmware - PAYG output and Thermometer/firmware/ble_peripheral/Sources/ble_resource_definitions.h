#ifndef	BLE_RESOURCE_DEFINITION_H_
#define BLE_RESOURCE_DEFINITION_H_

#include "ble_conn_params.h"
#include "nrf_sdh.h"
#include "peer_manager_handler.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"
#include "ble_dfu.h"
#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"

//SSL custom includes
#include "ble_pue_use_update_service.h"
#include "ble_device_config_service.h"
#include "ssl_config.h"

NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */

// SSL Custom services def
BLE_PUE_USE_SERVICE_DEF(m_pue_use_service);
BLE_DEVICE_CONFIG_SERVICE_DEF(m_device_config_service);
// SSL Custom services def end

static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                            /**< Handle of the current connection. */
void advertising_start(bool erase_bonds);                                    /**< Forward declaration of advertising start function */

// YOUR_JOB: Use UUIDs for service(s) used in your application.
static ble_uuid_t m_adv_uuids[] = {//{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
																		//{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
																		//{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE},
																		{BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}
																	};

void advertising_init(void);
void advertising_start(bool erase_bonds);
void gatt_init(void);
void ble_stack_init(void);
void peer_manager_init(void);
void delete_bonds(void);
void conn_params_error_handler(uint32_t nrf_error);
void conn_params_init(void);
void services_init(void);
void gap_params_init(void);
void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context);
void advertising_config_get(ble_adv_modes_config_t * p_config);
static void disconnect(uint16_t conn_handle, void * p_context);
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event);
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name);
void pm_evt_handler(pm_evt_t const * p_evt);
void nrf_qwr_error_handler(uint32_t nrf_error);

//void resources_enumerator_write_handler(uint16_t conn_handle, ble_device_discovery_service_t * p_device_discovery_service, char device_state[]);
void payg_write_handler(uint16_t conn_handle, ble_pue_use_service_t * p_pue_use_service, char payg_state[]);
void temperature_write_handler(uint16_t conn_handle, ble_pue_use_service_t * p_pue_use_service, char timeseries_state[]);

void device_provision_write_handler(uint16_t conn_handle, ble_device_config_service_t * p_device_config_service, char device_provision_data[]);
void client_provision_write_handler(uint16_t conn_handle, ble_device_config_service_t * p_device_config_service, char client_provision_data[]);
void on_adv_evt(ble_adv_evt_t ble_adv_evt);
void on_conn_params_evt(ble_conn_params_evt_t * p_evt);
void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context);
void advertising_update_data(void);
void adv_switch(bool cmd);
#endif 
