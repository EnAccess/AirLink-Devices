#include "ble_pue_use_update_service.h"
#include <string.h>
#include "nrf_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <nrf_delay.h>
#include "cbor_definitions.h"
#include "SEGGER_RTT.h"           
#include "nx_common.h"
#include "nx_keycode.h"
#include "ble_flash_fstorage_definitions.h"

static const uint8_t PayG_CharName[] = "pc";
static const uint8_t temperature_CharName[] = "temp";
		
uint8_t payg_data_to_read[100];
uint8_t payg_data_to_read_index;

uint8_t temperature_data_to_read[100];
uint8_t temperature_data_to_read_index;

uint16_t timeseries_connection_reset_flag = 0xffff;
static const uint16_t payg_connection_reset_flag = 0x0001;
static const uint16_t temp_connection_reset_flag = 0x0040;

/**@brief Helper Function for preparation of payG data to read from BLE
 * @param[in]   None.
 * @param[out]  None.
 */
static void update_payg_data_to_read(){
	  payg_data_to_read_index = 0;
		// Reset timeseries varibles with the current data
		timeseries.tpayg.payG_credit_remaining = payg.payG_credit_remaining;
		timeseries.tpayg.mode = payg.mode;
		timeseries.tpayg.last_payg_credit = payg.last_payg_credit;
		timeseries.tpayg.last_payG_calc = payg.last_payG_calc;
		timeseries.tpayg.last_payG_update = payg.last_payG_update;
		timeseries.tpayg.timeseries_index = payg.timeseries_index;
		strcpy(timeseries.tpayg.token,payg.token);
		timeseries.tpayg.timeseries_index = system_uptime;
		
		if(timeseries_connection_reset_flag&payg_connection_reset_flag){	// Connection reset give current data
			timeseries_connection_reset_flag &= ~payg_connection_reset_flag;
		}
		else if(update_data_by_marker(PAYG_PAGE) == false){
			NRF_LOG_INFO("PAYG No Update from Flash");	
			timeseries.tpayg.timeseries_index = 0;
			//return;
		}
		payg.timeseries_index = timeseries.tpayg.timeseries_index;
		
		// Add cbor header of map with 7 elements 
		payg_data_to_read_index = byte_cborArr_encode(payg_data_to_read,payg_data_to_read_index,10,false);
		// 1. Add cbor formated payg_resource_rtr (2+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"rtr",strlen("rtr"),true);
		payg_data_to_read_index = byte_cbor16_encode(payg_data_to_read,payg_data_to_read_index, payg.rtr);
		
		// 2. Add cbor formated AirLink PAYG resource version "rv" (2+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"rv",strlen("rv"),true);
		payg_data_to_read_index = byte_cbor32_encode(payg_data_to_read,payg_data_to_read_index, payg.resource_version);
		// 3. Add cbor formated payG_credit_remaining "cr->re" (4+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"re",strlen("re"),true);
		payg_data_to_read_index = byte_cbor32_encode(payg_data_to_read,payg_data_to_read_index, timeseries.tpayg.payG_credit_remaining);
		// 4. Add cbor formated payG_mode "mo" (1+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"mo",strlen("mo"),true);
		payg_data_to_read_index = byte_cbor8_encode(payg_data_to_read,payg_data_to_read_index, timeseries.tpayg.mode);
		// 5. Add cbor formated payg_credit_added "lcr" (2+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"lcr",strlen("lcr"),true);
		payg_data_to_read_index = byte_cbor16_encode(payg_data_to_read,payg_data_to_read_index, timeseries.tpayg.last_payg_credit);
		// 6. Add cbor formated last_payG_calc "ts" (4+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"pts",strlen("pts"),true);
		payg_data_to_read_index = byte_cbor32_encode(payg_data_to_read,payg_data_to_read_index, timeseries.tpayg.last_payG_calc);
		// 7. Add cbor formated last_payG_update "lts" (4+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"lts",strlen("lts"),true);
		payg_data_to_read_index = byte_cbor32_encode(payg_data_to_read,payg_data_to_read_index, timeseries.tpayg.last_payG_update);
		// 8. Add cbor formated timeseries_index "thi" (4+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"thi",strlen("thi"),true);
		payg_data_to_read_index = byte_cbor32_encode(payg_data_to_read,payg_data_to_read_index, timeseries.tpayg.timeseries_index);
		// 9. Add cbor formated token "tkn" (4+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"tkn",strlen("tkn"),true);
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)timeseries.tpayg.token,strlen(payg.token),true);
		// 10. Add cbor formated timeseries_timestamp "tts" (4+1 Byte)
		payg_data_to_read_index = byte_cborStr_encode(payg_data_to_read,payg_data_to_read_index, (uint8_t *)"ts",strlen("ts"),true);
		payg_data_to_read_index = byte_cbor32_encode(payg_data_to_read,payg_data_to_read_index, (timeseries.tpayg.timeseries_index*dev.time_calibration_factor[0])+dev.time_calibration_factor[1]);
		
		
		#ifdef DEBUG 
			nrf_delay_ms(500);
			SEGGER_RTT_printf(0,"payg:%d:",payg_data_to_read_index);
			for(int i=0;i<payg_data_to_read_index;i++){
				SEGGER_RTT_printf(0,"%02x ",payg_data_to_read[i]);
			}
			SEGGER_RTT_printf(0,"\r\n");
		#endif
		
}

/**@brief Helper Function for preparation of Temperature data to read from BLE
 * @param[in]   None.
 * @param[out]   None.
 */
static void update_temperature_data_to_read(){
	  temperature_data_to_read_index = 0;
		// Reset timeseries varibles with the current data
		timeseries.ttemp.max_temperature = temp.max_temperature;
		timeseries.ttemp.timeseries_index = system_uptime;
			
		if(timeseries_connection_reset_flag&temp_connection_reset_flag){	// Connection reset give current data
			timeseries_connection_reset_flag &= ~temp_connection_reset_flag;
		}
		else if(update_data_by_marker(TEMP_PAGE) == false){
			NRF_LOG_INFO("TEMP No Update from Flash");	
			timeseries.ttemp.timeseries_index = 0;
			//return;
		}
		temp.timeseries_index = timeseries.ttemp.timeseries_index;
	
		// Add cbor header of map with 4 elements 
		temperature_data_to_read_index = byte_cborArr_encode(temperature_data_to_read,temperature_data_to_read_index,6,false);
		// 1. Add cbor formated timeseries_resource_rtr (2+1 Byte)
		temperature_data_to_read_index = byte_cborStr_encode(temperature_data_to_read,temperature_data_to_read_index, (uint8_t *)"rtr",strlen("rtr"),true);
		temperature_data_to_read_index = byte_cbor16_encode(temperature_data_to_read,temperature_data_to_read_index, temp.rtr);
		// 2. Add cbor formated AirLink Current Temperature "temp" (4+1 Byte)
		temperature_data_to_read_index = byte_cborStr_encode(temperature_data_to_read,temperature_data_to_read_index, (uint8_t *)"temp",strlen("temp"),true);
		temperature_data_to_read_index = byte_cbor32_encode(temperature_data_to_read,temperature_data_to_read_index, timeseries.ttemp.current_temperature);
		// 3. Add cbor formated AirLink Max Temperature "tmax" (4+1 Byte)
		temperature_data_to_read_index = byte_cborStr_encode(temperature_data_to_read,temperature_data_to_read_index, (uint8_t *)"tmax",strlen("tmax"),true);
		temperature_data_to_read_index = byte_cbor32_encode(temperature_data_to_read,temperature_data_to_read_index, timeseries.ttemp.max_temperature);
		// 4. Add cbor formated AirLink Temperature upper limit threshold "tlim" (4+1 Byte)
		temperature_data_to_read_index = byte_cborStr_encode(temperature_data_to_read,temperature_data_to_read_index, (uint8_t *)"tlim",strlen("tlim"),true);
		temperature_data_to_read_index = byte_cbor32_encode(temperature_data_to_read,temperature_data_to_read_index, temp.threshold_temperature);
		// 5. Add cbor formated timeseries_index "thi" (4+1 Byte)
		temperature_data_to_read_index = byte_cborStr_encode(temperature_data_to_read,temperature_data_to_read_index, (uint8_t *)"thi",strlen("thi"),true);
		temperature_data_to_read_index = byte_cbor32_encode(temperature_data_to_read,temperature_data_to_read_index, timeseries.ttemp.timeseries_index);
		// 6. Add cbor formated timeseries_timestamp "tts" (4+1 Byte)
		temperature_data_to_read_index = byte_cborStr_encode(temperature_data_to_read,temperature_data_to_read_index, (uint8_t *)"ts",strlen("ts"),true);
		temperature_data_to_read_index = byte_cbor32_encode(temperature_data_to_read,temperature_data_to_read_index, (timeseries.ttemp.timeseries_index*dev.time_calibration_factor[0])+dev.time_calibration_factor[1]);
		
		#ifdef DEBUG 	
			SEGGER_RTT_printf(0,"Temperature count:%d:",temperature_data_to_read_index);
			for(int i=0;(i<temperature_data_to_read_index && i<100);i++){
				SEGGER_RTT_printf(0,"%02x ",temperature_data_to_read[i]);
			}
			nrf_delay_ms(500);
			SEGGER_RTT_printf(0,"\r\n");
		#endif
			
}

/**@brief Function for handling pue Read/Write Authorization event.
 * @brief Function updates characteristics value just before being read.
 * @param[in]   p_pue_use_service  PUE_USE service structure.
 * @param[in]   p_ble_evt      Event received from the BLE stack.
 */
static uint32_t ble_pue_data_resp_update(ble_pue_use_service_t * p_pue_use_service, ble_evt_t const * p_ble_evt)
{
  	uint32_t	err_code;
		ble_gatts_value_t gatts_value;
		ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
    p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_INVALID;
    p_rw_authorize_reply_params.params.read.update = 0; //1
    p_rw_authorize_reply_params.params.read.offset = 0;
    p_rw_authorize_reply_params.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
		ble_gatts_evt_read_t const * p_evt_read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
		uint8_t authenticated_length = 0;
		if(dfu.auth){									// If authenticated let the client read all the data otherwise read o
			authenticated_length = 0xff;// Don't change 
		}
		
		// PAYG Authorization request
		if(p_evt_read->handle == p_pue_use_service->payg_char_handles.value_handle){
			update_payg_data_to_read();
			gatts_value.len = payg_data_to_read_index & authenticated_length;
			gatts_value.offset  = 0;
			gatts_value.p_value = payg_data_to_read; 
			err_code = sd_ble_gatts_value_set(p_pue_use_service->conn_handle,p_pue_use_service->payg_char_handles.value_handle, &gatts_value);
			p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
			//NRF_LOG_INFO("PayG \n");
		}

		// Temperature Authorization request
		else if(p_evt_read->handle == p_pue_use_service->temperature_char_handles.value_handle){
			update_temperature_data_to_read();
			gatts_value.len = temperature_data_to_read_index & authenticated_length;
			gatts_value.offset  = 0;
			gatts_value.p_value = temperature_data_to_read; 
			err_code = sd_ble_gatts_value_set(p_pue_use_service->conn_handle,p_pue_use_service->temperature_char_handles.value_handle, &gatts_value);
			p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
			//NRF_LOG_INFO("Timeseries \n");
		}
		err_code = sd_ble_gatts_rw_authorize_reply(p_pue_use_service->conn_handle,&p_rw_authorize_reply_params);
		return err_code;
}


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_pue_use_service  PUE_USE service structure.
 * @param[in]   p_ble_evt      Event received from the BLE stack.
 */
static void on_connect(ble_pue_use_service_t * p_pue_use_service, ble_evt_t const * p_ble_evt)
{
    p_pue_use_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_bas       PUE_USE service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_pue_use_service_t * p_pue_use_service, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_pue_use_service->conn_handle = BLE_CONN_HANDLE_INVALID;
		//ble_dfu_pue_data_resp_update(p_pue_use_service);
}

/**@brief Function for handling the Write event.
 *
 * @param[in] p_pue_use_service   PUE_USE Service structure.
 * @param[in] p_ble_evt       Event received from the BLE stack.
 */
static void pue_use_on_write(ble_pue_use_service_t * p_pue_use_service, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
		// Write protection
		if(!dfu.auth)
			return;
		// payg write
		if ((p_evt_write->handle == p_pue_use_service->payg_char_handles.value_handle) && (p_pue_use_service->payg_write_handler != NULL)){
				// Convert Bytes to Array of characters
				bool no_err = false;
				if(1){
					uint32_t temp32_int = 0;
					uint8_t tempstr[22];

					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &tempstr, (uint8_t *)"tkn",p_evt_write->len);				// token
					if(no_err){		// Extract token
						pue.device_fault_data &= 0XFF00FFFF;
						if(strncmp((char *)payg.token,(char *)tempstr,16) != 0){
							strncpy((char *)payg.token,(char *)tempstr,16);
							NRF_LOG_INFO("Received tkn %s \n",payg.token);
							// Pass the keycode into the Nexus Keycode library 'all at once'.
							struct nx_keycode_complete_code keycode = {.keys = payg.token,
																												 .length = (uint8_t)strlen(payg.token)};

							NRF_LOG_INFO("\npassing along key=[%s] len=%d\n", keycode.keys, keycode.length);
							if (!nx_keycode_handle_complete_keycode(&keycode))
							{
									NRF_LOG_INFO("\tUnable to parse the keycode %s.\n", keycode.keys);
							}
							else{				
								no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"lt",p_evt_write->len);			// local time	timestamp
								if(no_err){		// Extract lt
									payg.last_payG_update = temp32_int;
									payg.local_time = temp32_int;
									NRF_LOG_INFO("Received lt %d \n",payg.local_time);
								}
							}
							flash_upload_flag |= PAYG_PAGE_FLAG;
							flash_upload_flag |= SYSTEM_TIME_PAGE_FLAG;
						}
						else{
							pue.device_fault_data |= (0x03<<16);
							NRF_LOG_INFO("Received duplicate tkn %s \n",payg.token);
						}
					}
				}
				else{
					NRF_LOG_INFO("Map error \n");
				}
				char data[] = {0};
				p_pue_use_service->payg_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_pue_use_service, data);
		}
		// Temperature
		else if ((p_evt_write->handle == p_pue_use_service->temperature_char_handles.value_handle) && (p_pue_use_service->temperature_write_handler != NULL)){
			bool no_err = false;
			uint8_t updates = 0;
				if(1){
					uint32_t temp32_int = 0;
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"tlim",p_evt_write->len);			// Temperature upper limit threshold "tlim"
					if(no_err){		// Extract tlim
						if(temp.threshold_temperature != temp32_int){
							updates++;
						}
						temp.threshold_temperature = temp32_int;
						NRF_LOG_INFO("Received tlim %d \n",temp.threshold_temperature);
					}
					
					char data[] = {updates};
					p_pue_use_service->temperature_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_pue_use_service, data);
				}
				else{
					NRF_LOG_INFO("Map error \n");
				}
		}
}

/**@brief Function for adding the PAYG characteristic.
 *
 */
static uint32_t payg_char_add(ble_pue_use_service_t * p_pue_use_service)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;
    ble_uuid_t          ble_uuid;

    memset(&char_md, 0, sizeof(char_md));
    memset(&attr_md, 0, sizeof(attr_md));
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    char_md.char_props.read          = 1;
    char_md.char_props.write         = 1;
    char_md.p_char_user_desc         = PayG_CharName;
    char_md.char_user_desc_size      = sizeof(PayG_CharName);
    char_md.char_user_desc_max_size  = sizeof(PayG_CharName);
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    // Define the PAYG Characteristic UUID
    ble_uuid.type = p_pue_use_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_PAYG_CHAR_UUID;

    // Set permissions on the Characteristic value
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);

    // Attribute Metadata settings
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;

		// Update attribute value
		update_payg_data_to_read();
		
    // Attribute Value settings
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = payg_data_to_read_index;//sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 100;//sizeof(uint8_t);
    attr_char_value.p_value      = payg_data_to_read;

    return sd_ble_gatts_characteristic_add(p_pue_use_service->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_pue_use_service->payg_char_handles);
}


/**@brief Function for adding the Temperature characteristic.
 *
 */
static uint32_t temperature_char_add(ble_pue_use_service_t * p_pue_use_service)
{
    ble_gatts_char_md_t char_md;
    ble_gatts_attr_t    attr_char_value;
    ble_gatts_attr_md_t attr_md;
    ble_uuid_t          ble_uuid;

    memset(&char_md, 0, sizeof(char_md));
    memset(&attr_md, 0, sizeof(attr_md));
    memset(&attr_char_value, 0, sizeof(attr_char_value));

    char_md.char_props.read          = 1;
    char_md.char_props.write         = 1;		
    char_md.p_char_user_desc         = temperature_CharName;
    char_md.char_user_desc_size      = sizeof(temperature_CharName);
    char_md.char_user_desc_max_size  = sizeof(temperature_CharName);
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    // Define the PUE Characteristic UUID
    ble_uuid.type = p_pue_use_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_TEMPERATURE_CHAR_UUID;

    // Set permissions on the Characteristic value
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);

    // Attribute Metadata settings
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
	
		// Update attribute value
		update_temperature_data_to_read();
		
    // Attribute Value settings
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = temperature_data_to_read_index;//sizeof(uint8_t);
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 100;//sizeof(uint8_t);
    attr_char_value.p_value      = temperature_data_to_read;

    return sd_ble_gatts_characteristic_add(p_pue_use_service->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_pue_use_service->temperature_char_handles);
}

uint32_t ble_pue_use_service_init(ble_pue_use_service_t * p_pue_use_service, const ble_pue_use_service_init_t * p_pue_use_service_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_pue_use_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    // Initialize service structure.
    p_pue_use_service->payg_write_handler = p_pue_use_service_init->payg_write_handler;
		p_pue_use_service->temperature_write_handler = p_pue_use_service_init->temperature_write_handler;	
	
    // Add service UUID
    ble_uuid128_t base_uuid = {BLE_UUID_PUE_USE_SERVICE_BASE_UUID};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_pue_use_service->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Set up the UUID for the service (base + service-specific)
    ble_uuid.type = p_pue_use_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_PUE_USE_SERVICE_UUID;

		// Set up and add the service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_pue_use_service->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add the different characteristics in the service:
    //   PAYG CHARACTERISTICS
    err_code = payg_char_add(p_pue_use_service);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		
		//   Temperature CHARACTERISTICS
		err_code = temperature_char_add(p_pue_use_service);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }		
		return NRF_SUCCESS;
}

void ble_pue_use_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_pue_use_service_t * p_pue_use_service = (ble_pue_use_service_t *)p_context;
		switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            timeseries_connection_reset_flag = 0xffff;
						on_connect(p_pue_use_service, p_ble_evt);
						break;

        case BLE_GATTS_EVT_WRITE:
            pue_use_on_write(p_pue_use_service, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_pue_use_service, p_ble_evt);
            break;
				
				case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
						ble_pue_data_resp_update(p_pue_use_service,p_ble_evt);
						break;
				
        default:
						break;
    }
		
}
