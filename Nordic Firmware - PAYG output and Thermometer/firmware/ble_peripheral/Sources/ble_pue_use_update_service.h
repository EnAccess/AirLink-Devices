#ifndef BLE_PUE_USE_UPDATE_service_H_
#define BLE_PUE_USE_UPDATE_service_H_


#include <stdint.h>
#include "boards.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "ssl_config.h"

/**@brief   Macro for defining a ble_pue_use_service instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_PUE_USE_SERVICE_DEF(_name)                                                                          \
static ble_pue_use_service_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                         \
                     BLE_PUE_USE_SERVICE_BLE_OBSERVER_PRIO,                                                     \
                     ble_pue_use_service_on_ble_evt, &_name)

// Forward declaration of the custom_service_t type.
typedef struct ble_pue_use_service_s ble_pue_use_service_t;

typedef void (*ble_pue_use_service_pue_use_write_handler_t) (uint16_t conn_handle, ble_pue_use_service_t * p_pue_use_service, char new_state[]);

/** @brief PAYG Service init structure. This structure contains all options and data needed for
 *        initialization of the service.*/
typedef struct
{
    ble_pue_use_service_pue_use_write_handler_t payg_write_handler; 				/**< Event handler to be called when the PAYG Characteristic is written. */
		ble_pue_use_service_pue_use_write_handler_t temperature_write_handler; 	/**< Event handler to be called when the Timeseries Characteristic is written. */
} ble_pue_use_service_init_t;

/**@brief PAYG Service structure.
 *        This contains various status information
 *        for the service.
 */
typedef struct ble_pue_use_service_s
{
    uint16_t                            conn_handle;
    uint16_t                            service_handle;
    uint8_t                             uuid_type;
    ble_gatts_char_handles_t            payg_char_handles;
    ble_pue_use_service_pue_use_write_handler_t payg_write_handler;
		
		ble_gatts_char_handles_t            temperature_char_handles;
    ble_pue_use_service_pue_use_write_handler_t temperature_write_handler;
} ble_pue_use_service_t;

// Function Declarations

/**@brief Function for initializing the PAYG Service.
 *
 * @param[out]  p_pue_use_service  PAYG Service structure. This structure will have to be supplied by
 *                                the application. It will be initialized by this function, and will later
 *                                be used to identify this particular service instance.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_pue_use_service_init(ble_pue_use_service_t * p_pue_use_service, const ble_pue_use_service_init_t * p_pue_use_service_init);

/**@brief Function for handling the application's BLE stack events.
 *
 * @details This function handles all events from the BLE stack that are of interest to the PAYG Service.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 * @param[in] p_context  PAYG Service structure.
 */
void ble_pue_use_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);
#endif // BLE_PAYG_UPDATE_service_H_
