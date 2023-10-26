#ifndef BLE_DEVICE_CONFIG_SERVICE_H_
#define BLE_DEVICE_CONFIG_SERVICE_H_


#include <stdint.h>
#include "boards.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "ssl_config.h"

/**@brief   Macro for defining a ble_device_config_service instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */

#define BLE_DEVICE_CONFIG_SERVICE_DEF(_name)                                                                          \
static ble_device_config_service_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                         \
                     BLE_DEVICE_CONFIG_SERVICE_BLE_OBSERVER_PRIO,                                                     \
                     ble_device_config_service_on_ble_evt, &_name)

// Forward declaration of the custom_service_t type.
typedef struct ble_device_config_service_s ble_device_config_service_t;

typedef void (*ble_device_config_service_device_config_write_handler_t) (uint16_t conn_handle, ble_device_config_service_t * p_device_config_service, char new_data[]);

/** @brief PROVISION Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_device_config_service_device_config_write_handler_t device_provision_write_handler; /**< Event handler to be called when the DEVICE PROVISION Characteristic is written. */
		ble_device_config_service_device_config_write_handler_t client_provision_write_handler; /**< Event handler to be called when the CLIENT PROVISION Characteristic is written. */
} ble_device_config_service_init_t;

/**@brief PROVISION Service structure.
 *        This contains various status information
 *        for the service.
 */
typedef struct ble_device_config_service_s
{
    uint16_t                            conn_handle;
    uint16_t                            service_handle;
    uint8_t                             uuid_type;
		ble_gatts_char_handles_t            device_provision_char_handles;
		ble_gatts_char_handles_t            client_provision_char_handles;	
    ble_device_config_service_device_config_write_handler_t device_provision_write_handler;
		ble_device_config_service_device_config_write_handler_t client_provision_write_handler;

} ble_device_config_service_t;

// Function Declarations

/**@brief Function for initializing the PROVISION Service.
 *
 * @param[out]  p_device_config_service  PROVISION Service structure. This structure will have to be supplied by
 *                                the application. It will be initialized by this function, and will later
 *                                be used to identify this particular service instance.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_device_config_service_init(ble_device_config_service_t * p_device_config_service, const ble_device_config_service_init_t * p_device_config_service_init);

/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the PROVISION Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  PROVISION Service structure.
 */
void ble_device_config_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

#endif // BLE_DEVICE_CONFIG_SERVICE_H_
