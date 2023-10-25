/*This file contains defitions for ble Adverts and Services library*/
#include "ble_resource_definitions.h"
#include "ble_pwr_mgmt_definitions.h"
#include <stdlib.h>
#include "nrf_power.h"
#include "nrf_log.h"
#include "nrf_bootloader_info.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "bsp_btn_ble.h"
#include "nrf_delay.h"
#include "ble_flash_fstorage_definitions.h" 
#include "cbor_definitions.h"
#include "SEGGER_RTT.h" 
#include "nxp_common.h"

static bool adv_stopped = false;
static bool ble_connected = false;

/**@brief Function for initializing the Advertising functionality.
 */
static bool advertising_data_get_update(ble_advdata_manuf_data_t * manuf_data_response){
		
		// Prepare the scan response manufacturer specific data packet
    //ble_advdata_manuf_data_t  manuf_data_response;
    //uint8_t data_response[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"; // 26 Characters MAX
		
		uint8_t index=0;
		static uint8_t ptr[26],ptr_old[26];
		//ptr = (uint8_t *)malloc(26*sizeof(uint8_t));
		
		// Add cbor header of array with 8 elements 
		index = byte_cborArr_encode(ptr,index,8,true);
		
		// 1. Add cbor formated advt_data_format (2+1 Byte)
		index = byte_cbor16_encode(ptr,index, advert.resource_version);
		
		// 2. Add cbor formated error status (1+0 Byte)
		index = byte_cbor8_encode(ptr,index, pue.device_fault);
		
		// 3. Add cbor formated device_id (4+1 Byte)
		index = byte_cbor32_encode(ptr,index, dev.id);
		
		// 4. Add cbor formated last_payG_update (4+1 Byte)
		index = byte_cbor32_encode(ptr,index, payg.last_payG_update);
		
		// 5. Add cbor formated dev_asset_status (1+0 Byte)
		index = byte_cbor8_encode(ptr,index, dev.provisioning_status);
		
		// 6. Add cbor formated sw_version (1+1 Byte)
		index = byte_cbor8_encode(ptr,index, dfu.fw_version);
		
		// 7. Add cbor formated payG_credit_remaining (4+1 Byte)
		//index = byte_cbor32_encode(ptr,index, payg.payG_credit_remaining);
		index = byte_cbor32_encode(ptr,index, nxp_common_payg_credit_get_remaining());		
		
		//uint8_t pu[] = {dev.payg_unit[0]};
		// 8. Add cbor formated payG_unit (1+1 Byte)
		index = byte_cborStr_encode(ptr,index, (uint8_t*)dev.payg_unit,strlen(dev.payg_unit),true);
		
		bool return_value = false;
		// Check if update is needed
		if(memcmp(ptr,ptr_old,26) != 0){
			memcpy(ptr_old,ptr,26);
			return_value = true;
			#ifdef DEBUG 
			//#include "SEGGER_RTT.h"
			SEGGER_RTT_printf(0,"Adv:");		
			for (int i=0;i < index;i++) 
				SEGGER_RTT_printf(0,"%02x ",ptr[i]);
			SEGGER_RTT_printf(0,"\r\n");		
			#endif
		}
		manuf_data_response->company_identifier  = 0x0059;
    manuf_data_response->data.p_data         = ptr;
    manuf_data_response->data.size           = index;
    //free(ptr);
		return return_value;
}

/**@brief Function for initializing the Advertising functionality.
 */
void advertising_init(void)
{
    uint32_t               err_code;
    ble_advertising_init_t init;

    memset(&init, 0, sizeof(init));
		
	  //Set manufacturing data
		/* Disabled advertising manufacturer data to allow full mane advertising
		* Utilizing scan response instead
    ble_advdata_manuf_data_t                  manuf_data; //Variable to hold manufacturer specific data
    uint8_t data[]                            = "";  //Our data to advertise (1 BYTES)
		manuf_data.company_identifier             =  0x0059; //Nordics company ID
    manuf_data.data.p_data                    = data;
    manuf_data.data.size                      = sizeof(data);
    init.advdata.p_manuf_specific_data = &manuf_data;
		*/

	  //Build advertising data struct to pass into @ble_advertising_init
    init.advdata.name_type               = BLE_ADVDATA_FULL_NAME;
    init.advdata.include_appearance      = true;
    init.advdata.flags                   = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;
		
		ble_advdata_manuf_data_t   manuf_data_response_to_send;
		advertising_data_get_update(&manuf_data_response_to_send);
		init.srdata.name_type = BLE_ADVDATA_NO_NAME;
    init.srdata.p_manuf_specific_data = &manuf_data_response_to_send;
		
    advertising_config_get(&init.config);

    init.evt_handler = on_adv_evt;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
		////free(data_ptr);
		

}

/**@brief Function for updating advertising.
 */
void advertising_update_data(void)
{
  /* Declaration of Error Code */
  ret_code_t err_code;
  ble_advdata_t   advdata,srdata;
	
	memset(&advdata, 0, sizeof(advdata));

  advdata.name_type = BLE_ADVDATA_FULL_NAME;
  advdata.include_appearance = true;
  advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;

  advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
  advdata.uuids_complete.p_uuids = &m_adv_uuids[0];

	memset(&srdata, 0, sizeof(srdata));
	ble_advdata_manuf_data_t   manuf_data_response_to_send;
	// If there is no data change do not update
	if(advertising_data_get_update(&manuf_data_response_to_send) == false){
		return;
	}
	if(adv_stopped){
		return;
	}
	
	srdata.name_type = BLE_ADVDATA_NO_NAME;
  srdata.p_manuf_specific_data = &manuf_data_response_to_send;
		  
  //advdata.p_service_data_array = &service_data;  // Pointer to Service Data structure.
  //advdata.service_data_count = 1;

  //sd_ble_gap_adv_stop(m_adv_handle);
	//sd_ble_gap_adv_stop(m_advertising.adv_handle);
	
  err_code = ble_advertising_advdata_update(&m_advertising, &advdata, &srdata);
  APP_ERROR_CHECK(err_code);
}

void adv_switch(bool cmd){
	 
	ret_code_t err_code;
	if(cmd && adv_stopped){		// switch on ADV from off position
		//advertising_init();
		//advertising_start(true);
		SEGGER_RTT_printf(0,"advertising is started\r\n");
		
		err_code = sd_ble_gap_adv_start(m_advertising.adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
    //NRF_LOG_INFO("Starting advertising");
        
		
		adv_stopped = false;
	}
	else if(!cmd && !adv_stopped){	// switch off ADV from on position
		//sd_ble_gap_adv_stop(m_advertising.adv_handle);
		//NRF_LOG_DEBUG("advertising is stopped");
		static uint32_t disconnect_timeout = 0;
		if(ble_connected && m_conn_handle != BLE_CONN_HANDLE_INVALID){
			err_code = sd_ble_gap_disconnect(m_conn_handle,BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
      APP_ERROR_CHECK(err_code);
			SEGGER_RTT_printf(0,"Disconnected\r\n");
			//nrf_delay_ms(10);
			disconnect_timeout = (system_uptime*60)+(NRF_RTC2->COUNTER/8)+5;
			m_conn_handle = BLE_CONN_HANDLE_INVALID;
		}
		//SEGGER_RTT_printf(0,"Here: %d %d\r\n",(system_uptime*60)+(NRF_RTC2->COUNTER/8),disconnect_timeout);
		if(disconnect_timeout>((system_uptime*60)+(NRF_RTC2->COUNTER/8))){
			return;
		}
		
		err_code = sd_ble_gap_adv_stop(m_advertising.adv_handle);
    APP_ERROR_CHECK(err_code);
    SEGGER_RTT_printf(0,"advertising is stopped\r\n");
		//NRF_LOG_INFO("Stopping advertising");
    disconnect_timeout = (system_uptime*60)+(NRF_RTC2->COUNTER/8)+5;    
		adv_stopped = true;
	}
	else{
		;
	}
}

/**@brief Function for starting advertising.
 */
void advertising_start(bool erase_bonds)
{
    if (erase_bonds == true)
    {
        delete_bonds();
        // Advertising is started by PM_EVT_PEERS_DELETE_SUCCEEDED event.
    }
    else
    {
        uint32_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
        APP_ERROR_CHECK(err_code);

        NRF_LOG_DEBUG("advertising is started");
    }
}

/**@brief   Function for initializing the GATT module.
 * @details The GATT module handles ATT_MTU and Data Length update procedures automatically.
 */
void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, NULL);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for initializing the BLE stack.
 *
 * @details Initializes the SoftDevice and the BLE event interrupt.
 */
void ble_stack_init(void)
{
    ret_code_t err_code;

    err_code = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err_code);

    // Configure the BLE stack using the default settings.
    // Fetch the start address of the application RAM.
    uint32_t ram_start = 0;
    err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
    APP_ERROR_CHECK(err_code);

    // Enable BLE stack.
    err_code = nrf_sdh_ble_enable(&ram_start);
    APP_ERROR_CHECK(err_code);

    NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, ble_evt_handler, NULL);
}


/**@brief Function for the Peer Manager initialization.
 */
void peer_manager_init(void)
{
    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

    memset(&sec_param, 0, sizeof(ble_gap_sec_params_t));

    // Security parameters to be used for all security procedures.
    sec_param.bond           = SEC_PARAM_BOND;
    sec_param.mitm           = SEC_PARAM_MITM;
    sec_param.lesc           = SEC_PARAM_LESC;
    sec_param.keypress       = SEC_PARAM_KEYPRESS;
    sec_param.io_caps        = SEC_PARAM_IO_CAPABILITIES;
    sec_param.oob            = SEC_PARAM_OOB;
    sec_param.min_key_size   = SEC_PARAM_MIN_KEY_SIZE;
    sec_param.max_key_size   = SEC_PARAM_MAX_KEY_SIZE;
    sec_param.kdist_own.enc  = 1;
    sec_param.kdist_own.id   = 1;
    sec_param.kdist_peer.enc = 1;
    sec_param.kdist_peer.id  = 1;

    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);

    err_code = pm_register(pm_evt_handler);
    APP_ERROR_CHECK(err_code);
}


/** @brief Clear bonding information from persistent storage.
 */
void delete_bonds(void)
{
    ret_code_t err_code;

    NRF_LOG_INFO("Erase bonds!");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
void conn_params_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}


/**@brief Function for initializing the Connection Parameters module.
 */
void conn_params_init(void)
{
    uint32_t               err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}


/**@brief Function for handling the YYY Service events.
 * YOUR_JOB implement a service handler function depending on the event the service you are using can generate
 *
 * @details This function will be called for all YY Service events which are passed to
 *          the application.
 *
 * @param[in]   p_yy_service   YY Service structure.
 * @param[in]   p_evt          Event received from the YY Service.
 *
 *
   static void on_yys_evt(ble_yy_service_t     * p_yy_service,
                          ble_yy_service_evt_t * p_evt)
   {
    switch (p_evt->evt_type)
    {
        case BLE_YY_NAME_EVT_WRITE:
            APPL_LOG("[APPL]: charact written with value %s. ", p_evt->params.char_xx.value.p_str);
            break;

        default:
            // No implementation needed.
            break;
    }
   }*/

/**@brief Function for initializing services that will be used by the application.
 */
void services_init(void)
{
    uint32_t                  err_code;
    nrf_ble_qwr_init_t        qwr_init  = {0};
    ble_dfu_buttonless_init_t dfus_init = {0};

    // Initialize Queued Write Module.
    qwr_init.error_handler = nrf_qwr_error_handler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);

    dfus_init.evt_handler = ble_dfu_evt_handler;

    err_code = ble_dfu_buttonless_init(&dfus_init);
    APP_ERROR_CHECK(err_code);

		
		// Initialize PUE_USE service
		ble_pue_use_service_init_t pue_use_init;
    pue_use_init.payg_write_handler = payg_write_handler;
		pue_use_init.temperature_write_handler = temperature_write_handler;
		
    err_code = ble_pue_use_service_init(&m_pue_use_service, &pue_use_init);
    APP_ERROR_CHECK(err_code);
		
		// Initialize DEVICE_CONFIG service
		ble_device_config_service_init_t device_config_init;
    device_config_init.device_provision_write_handler = device_provision_write_handler;
		device_config_init.client_provision_write_handler = client_provision_write_handler;
		
    err_code = ble_device_config_service_init(&m_device_config_service, &device_config_init);
    APP_ERROR_CHECK(err_code);
		//*/
		
    /* YOUR_JOB: Add code to initialize the services used by the application.
       uint32_t                           err_code;
       ble_xxs_init_t                     xxs_init;
       ble_yys_init_t                     yys_init;

       // Initialize XXX Service.
       memset(&xxs_init, 0, sizeof(xxs_init));

       xxs_init.evt_handler                = NULL;
       xxs_init.is_xxx_notify_supported    = true;
       xxs_init.ble_xx_initial_value.level = 100;

       err_code = ble_bas_init(&m_xxs, &xxs_init);
       APP_ERROR_CHECK(err_code);

       // Initialize YYY Service.
       memset(&yys_init, 0, sizeof(yys_init));
       yys_init.evt_handler                  = on_yys_evt;
       yys_init.ble_yy_initial_value.counter = 0;

       err_code = ble_yy_service_init(&yys_init, &yy_init);
       APP_ERROR_CHECK(err_code);
     */
}


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */
void gap_params_init(void)
{
    uint32_t                err_code;
    ble_gap_conn_params_t   gap_conn_params;
    ble_gap_conn_sec_mode_t sec_mode;

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);

    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

    /* YOUR_JOB: Use an appearance value matching the application's use case.
       err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_);
       APP_ERROR_CHECK(err_code); */

    memset(&gap_conn_params, 0, sizeof(gap_conn_params)) ;

    gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
    gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
    gap_conn_params.slave_latency     = SLAVE_LATENCY;
    gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

void buttonless_dfu_sdh_state_observer(nrf_sdh_state_evt_t state, void * p_context)
{
    if (state == NRF_SDH_EVT_STATE_DISABLED)
    {
        // Softdevice was disabled before going into reset. Inform bootloader to skip CRC on next boot.
        nrf_power_gpregret2_set(BOOTLOADER_DFU_SKIP_CRC);
				
        //Go to system off.
        nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_SYSOFF);
    }
}

/* nrf_sdh state observer. */
NRF_SDH_STATE_OBSERVER(m_buttonless_dfu_state_obs, 0) =
{
    .handler = buttonless_dfu_sdh_state_observer,
};


void advertising_config_get(ble_adv_modes_config_t * p_config)
{
    memset(p_config, 0, sizeof(ble_adv_modes_config_t));

    p_config->ble_adv_fast_enabled  = true;
    p_config->ble_adv_fast_interval = APP_ADV_INTERVAL;
    p_config->ble_adv_fast_timeout  = APP_ADV_DURATION;
}


static void disconnect(uint16_t conn_handle, void * p_context)
{
    UNUSED_PARAMETER(p_context);

    ret_code_t err_code = sd_ble_gap_disconnect(conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
    if (err_code != NRF_SUCCESS)
    {
        NRF_LOG_WARNING("Failed to disconnect connection. Connection handle: %d Error: %d", conn_handle, err_code);
    }
    else
    {
        NRF_LOG_DEBUG("Disconnected connection handle %d", conn_handle);
    }
}


// YOUR_JOB: Update this code if you want to do anything given a DFU event (optional).
/**@brief Function for handling dfu events from the Buttonless Secure DFU service
 *
 * @param[in]   event   Event from the Buttonless Secure DFU service.
 */
static void ble_dfu_evt_handler(ble_dfu_buttonless_evt_type_t event)
{
    switch (event)
    {
        case BLE_DFU_EVT_BOOTLOADER_ENTER_PREPARE:
        {
            NRF_LOG_INFO("Device is preparing to enter bootloader mode.");

            // Prevent device from advertising on disconnect.
            ble_adv_modes_config_t config;
            advertising_config_get(&config);
            config.ble_adv_on_disconnect_disabled = true;
            ble_advertising_modes_config_set(&m_advertising, &config);

            // Disconnect all other bonded devices that currently are connected.
            // This is required to receive a service changed indication
            // on bootup after a successful (or aborted) Device Firmware Update.
            uint32_t conn_count = ble_conn_state_for_each_connected(disconnect, NULL);
            NRF_LOG_INFO("Disconnected %d links.", conn_count);
            break;
        }

        case BLE_DFU_EVT_BOOTLOADER_ENTER:
            // YOUR_JOB: Write app-specific unwritten data to FLASH, control finalization of this
            //           by delaying reset by reporting false in app_shutdown_handler
            NRF_LOG_INFO("Device will enter bootloader mode.");
            break;

        case BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED:
            NRF_LOG_ERROR("Request to enter bootloader mode failed asynchroneously.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            break;

        case BLE_DFU_EVT_RESPONSE_SEND_ERROR:
            NRF_LOG_ERROR("Request to send a response to client failed.");
            // YOUR_JOB: Take corrective measures to resolve the issue
            //           like calling APP_ERROR_CHECK to reset the device.
            APP_ERROR_CHECK(false);
            break;

        default:
            NRF_LOG_ERROR("Unknown event from ble_dfu_buttonless.");
            break;
    }
}


/**@brief Callback function for asserts in the SoftDevice.
 *
 * @details This function will be called in case of an assert in the SoftDevice.
 *
 * @warning This handler is an example only and does not fit a final product. You need to analyze
 *          how your product is supposed to react in case of Assert.
 * @warning On assert from the SoftDevice, the system can only recover on reset.
 *
 * @param[in] line_num   Line number of the failing ASSERT call.
 * @param[in] file_name  File name of the failing ASSERT call.
 */
void assert_nrf_callback(uint16_t line_num, const uint8_t * p_file_name)
{
    app_error_handler(DEAD_BEEF, line_num, p_file_name);
}


/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
void pm_evt_handler(pm_evt_t const * p_evt)
{
    pm_handler_on_pm_evt(p_evt);
    pm_handler_flash_clean(p_evt);
}




/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling resources_enumerator Write.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * 
 */
//void resources_enumerator_write_handler(uint16_t conn_handle, ble_device_discovery_service_t * p_device_discovery_service, char resources_enumerator_state[])
//{
//			//NRF_LOG_DEBUG("PayG update received\r\n");
//}

/**@brief Function for handling payg Write.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * 
 */
void payg_write_handler(uint16_t conn_handle, ble_pue_use_service_t * p_pue_use_service, char payg_state[])
{
			NRF_LOG_DEBUG("PayG update received\r\n");
}

void temperature_write_handler(uint16_t conn_handle, ble_pue_use_service_t * p_pue_use_service, char timeseries_state[])
{
		if(timeseries_state[0]){ 
			NRF_LOG_DEBUG("Temperature update:Flash needs update\r\n");
			flash_upload_flag |= CONFIG_PAGE_FLAG;
		}
		else{
			NRF_LOG_DEBUG("Temperature update:No flash update needed\r\n");
		}	
}

/**@brief Function for handling provision Write.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * 
 */

void device_provision_write_handler(uint16_t conn_handle, ble_device_config_service_t * p_device_config_service, char provision_data[])
{
		if(provision_data[0]){ 
			NRF_LOG_DEBUG("Device provision:Flash needs update\r\n");
			flash_upload_flag |= CONFIG_PAGE_FLAG;
		}
		else{
			NRF_LOG_DEBUG("Device provision:No flash update needed\r\n");
		}
}
void client_provision_write_handler(uint16_t conn_handle, ble_device_config_service_t * p_device_config_service, char provision_data[])
{
		if(provision_data[0]){ 
			NRF_LOG_DEBUG("Client provision:Flash needs update\r\n");
			flash_upload_flag |= CONFIG_PAGE_FLAG;
	  }
	  else{
			NRF_LOG_DEBUG("Client provision:No flash update needed\r\n");
	  }
	 
}

/**@brief Function for handling the Connection Parameters Module.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail config parameter, but instead we use the event
 *                handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    uint32_t err_code;

    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling advertising events.
 *
 * @details This function will be called for advertising events which are passed to the application.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */
void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
    uint32_t err_code;

    switch (ble_adv_evt)
    {
        case BLE_ADV_EVT_FAST:
            err_code = bsp_indication_set(BSP_INDICATE_ADVERTISING);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_ADV_EVT_IDLE:
            //sleep_mode_enter();
            break;

        default:
            break;
    }
}


/**@brief Function for handling BLE events.
 *
 * @param[in]   p_ble_evt   Bluetooth stack event.
 * @param[in]   p_context   Unused.
 */
void ble_evt_handler(ble_evt_t const * p_ble_evt, void * p_context)
{
    uint32_t err_code = NRF_SUCCESS;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_DISCONNECTED:
            // LED indication will be changed when advertising starts.
						NRF_LOG_INFO("Disconnected \n");
						dfu.auth = false;
						ble_connected = false;
            break;

        case BLE_GAP_EVT_CONNECTED:
            err_code = bsp_indication_set(BSP_INDICATE_CONNECTED);
            APP_ERROR_CHECK(err_code);
            m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
            err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
            APP_ERROR_CHECK(err_code);
						NRF_LOG_INFO("Connected \n");
						dfu.auth = false;
						ble_connected = true;
            break;
				
        case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
        {
            NRF_LOG_DEBUG("PHY update request.");
            ble_gap_phys_t const phys =
            {
                .rx_phys = BLE_GAP_PHY_AUTO,
                .tx_phys = BLE_GAP_PHY_AUTO,
            };
            err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
            APP_ERROR_CHECK(err_code);
            break;
        }

        case BLE_GATTC_EVT_TIMEOUT:
            // Disconnect on GATT Client timeout event.
            NRF_LOG_DEBUG("GATT Client Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        case BLE_GATTS_EVT_TIMEOUT:
            // Disconnect on GATT Server timeout event.
            NRF_LOG_DEBUG("GATT Server Timeout.");
            err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            APP_ERROR_CHECK(err_code);
            break;

        default:
            // No implementation needed.
            break;
    }
}
