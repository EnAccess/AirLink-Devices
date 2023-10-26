#include "ble_device_config_service.h"
#include <string.h>
#include "nrf_log.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <nrf_delay.h>
#include "cbor_definitions.h"
#include "SEGGER_RTT.h"           
#include "ble_pwr_mgmt_definitions.h"
#include "identity.h"

static const uint8_t Device_Provision_CharName[] = "dcfg";
static const uint8_t Client_Provision_CharName[] = "ccfg";
//static const uint8_t DFU_CharName[] = "DFU Data";

uint8_t device_provision_data_to_read[100];
uint8_t device_provision_data_to_read_index;
uint8_t client_provision_data_to_read[100];
uint8_t client_provision_data_to_read_index;

/**@brief Helper Function for preparation of device_provision data to read from BLE
 * @param[in]   None.
 * @param[out]  None.
 */
static void update_device_provision_data_to_read(){
	  device_provision_data_to_read_index = 0;
		// Add cbor header of map with 5 elements 
		device_provision_data_to_read_index = byte_cborArr_encode(device_provision_data_to_read,device_provision_data_to_read_index,8,false);
		// 1. Add cbor formated payg_resource_rtr (2+1 Byte)
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)"rtr",strlen("rtr"),true);
		device_provision_data_to_read_index = byte_cbor16_encode(device_provision_data_to_read,device_provision_data_to_read_index, dev.rtr);
		
		// 2. Add cbor formated AirLink device_provision resource version "rv" (2+1 Byte)
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)"rv",strlen("rv"),true);
		device_provision_data_to_read_index = byte_cbor32_encode(device_provision_data_to_read,device_provision_data_to_read_index, dev.resource_version);
		// 3. Add cbor formated AirLink Device ID "did" (4+1 Byte)
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)"did",strlen("did"),true);
		device_provision_data_to_read_index = byte_cbor32_encode(device_provision_data_to_read,device_provision_data_to_read_index, dev.id);
		// 4. Add cbor formated AirLink PayG Units accepted "pul" (?+1 Byte)
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)"pul",strlen("pul"),true);
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)dev.payg_units_accepted,strlen(dev.payg_units_accepted),true);
		// 5. Add cbor formated BLE TimeSeries Data Format "tdf" (1+1 Byte)
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)"tdf",strlen("tdf"),true);
		device_provision_data_to_read_index = byte_cbor8_encode(device_provision_data_to_read,device_provision_data_to_read_index, *dev.timeseries_data_format);
		// 6. Add cbor formated AirLink Provisioning Status "pst" (1+1 Byte)
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)"pst",strlen("pst"),true);
		device_provision_data_to_read_index = byte_cbor8_encode(device_provision_data_to_read,device_provision_data_to_read_index, dev.provisioning_status);
		// 7  Add cbor formated AirLink OpenPAYGO Metrics Data format "df" (2+1 Byte)
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)"df",strlen("df"),true);
		device_provision_data_to_read_index = byte_cbor16_encode(device_provision_data_to_read,device_provision_data_to_read_index, dev.openPAYGO_data_format);
		// 8	Add cbor formated AirLink productive_output_set_limit "opmax"  (4+1 Byte)
		device_provision_data_to_read_index = byte_cborStr_encode(device_provision_data_to_read,device_provision_data_to_read_index, (uint8_t *)"opmax",strlen("opmax"),true);
		device_provision_data_to_read_index = byte_cbor32_encode(device_provision_data_to_read,device_provision_data_to_read_index, dev.productive_output_set_limit);		
		
		#ifdef DEBUG 
			//SEGGER_RTT_printf(0,"device_provision:");
			SEGGER_RTT_printf(0,"device_provision:%d:",device_provision_data_to_read_index);
			for(int i=0;i<device_provision_data_to_read_index;i++){
				SEGGER_RTT_printf(0,"%02x ",device_provision_data_to_read[i]);
			}
			nrf_delay_ms(1000);
			SEGGER_RTT_printf(0,"\r\n");
		#endif
}

/**@brief Helper Function for preparation of client_provision data to read from BLE
 * @param[in]   None.
 * @param[out]  None.
 */
static void update_client_provision_data_to_read(){
	  client_provision_data_to_read_index = 0;
		// Add cbor header of map with 5 elements 
		client_provision_data_to_read_index = byte_cborArr_encode(client_provision_data_to_read,client_provision_data_to_read_index,6,false);
		// 1. Add cbor formated payg_resource_rtr (2+1 Byte)
		client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)"rtr",strlen("rtr"),true);
		client_provision_data_to_read_index = byte_cbor16_encode(client_provision_data_to_read,client_provision_data_to_read_index, cust.rtr);
		
		// 2. Add cbor formated AirLink device_provision resource version "rv" (2+1 Byte)
		client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)"rv",strlen("rv"),true);
		client_provision_data_to_read_index = byte_cbor32_encode(client_provision_data_to_read,client_provision_data_to_read_index, cust.resource_version);
		// 3. Add cbor formated AirLink Customer Name "cn" (16+1 Byte)
		client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)"cn",strlen("cn"),true);
		client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)cust.name,strlen(cust.name),true);
		// 4. Add cbor formated AirLink Customer's Phone "cp" (16+1 Byte)
		client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)"cp",strlen("cp"),true);
		client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)cust.phone,strlen(cust.phone),true);
		// 5. Add cbor formated BLE Readable ID "rid" (4+1 Byte)
		client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)"rid",strlen("rid"),true);
		client_provision_data_to_read_index = byte_cbor32_encode(client_provision_data_to_read,client_provision_data_to_read_index, cust.readable_id);
		// 6. Add cbor formated AirLink Provisioning Status "pst" (1+1 Byte)
		client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)"pst",strlen("pst"),true);
		client_provision_data_to_read_index = byte_cbor8_encode(client_provision_data_to_read,client_provision_data_to_read_index, *cust.provisioning_status);
		//// 7. Add cbor formated AirLink Server Auth Token "sat" (20+1 Byte)
		//client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, (uint8_t *)"sat",strlen("sat"),true);
		//client_provision_data_to_read_index = byte_cborStr_encode(client_provision_data_to_read,client_provision_data_to_read_index, cust.server_auth_token,strlen((char *)cust.server_auth_token),true);
		
		#ifdef DEBUG 
			//SEGGER_RTT_printf(0,"client_provision:");
			SEGGER_RTT_printf(0,"client_provision:%d:",client_provision_data_to_read_index);

			for(int i=0;i<client_provision_data_to_read_index;i++){
				SEGGER_RTT_printf(0,"%02x ",client_provision_data_to_read[i]);
			}
			nrf_delay_ms(1000);
			SEGGER_RTT_printf(0,"\r\n");
		#endif
}



static uint32_t ble_device_config_data_resp_update(ble_device_config_service_t * p_device_config_service, ble_evt_t const * p_ble_evt)
{
  	uint32_t	err_code;
		ble_gatts_value_t gatts_value;
		ble_gatts_rw_authorize_reply_params_t p_rw_authorize_reply_params;
    p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_INVALID;
    p_rw_authorize_reply_params.params.read.update = 0; //1
    p_rw_authorize_reply_params.params.read.offset = 0;
    p_rw_authorize_reply_params.params.read.gatt_status = BLE_GATT_STATUS_SUCCESS;
	
		p_rw_authorize_reply_params.params.write.update = 0; //1
    p_rw_authorize_reply_params.params.write.offset = 0;
    p_rw_authorize_reply_params.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
		
		ble_gatts_evt_read_t const * p_evt_read = &p_ble_evt->evt.gatts_evt.params.authorize_request.request.read;
		//ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
		
		//NRF_LOG_DEBUG("log: %d %d\n",p_evt_read->handle, p_evt_write->handle);
		uint8_t authenticated_length = 0;
		if(dfu.auth){									// If authenticated let the client read all the data otherwise read o
			authenticated_length = 0xff;// Don't change 
		}
	
		// Device provision Read Authorization request
		if(p_evt_read->handle == p_device_config_service->device_provision_char_handles.value_handle){
			update_device_provision_data_to_read();
			gatts_value.len = device_provision_data_to_read_index & authenticated_length;
			gatts_value.offset  = 0;
			gatts_value.p_value = device_provision_data_to_read; 
			err_code = sd_ble_gatts_value_set(p_device_config_service->conn_handle,p_device_config_service->device_provision_char_handles.value_handle, &gatts_value);
			p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
			//NRF_LOG_INFO("PayG \n");
		}
		// Client provision Read Authorization request
		else if(p_evt_read->handle == p_device_config_service->client_provision_char_handles.value_handle){
			update_client_provision_data_to_read();
			gatts_value.len = client_provision_data_to_read_index & authenticated_length;
			gatts_value.offset  = 0;
			gatts_value.p_value = client_provision_data_to_read; 
			err_code = sd_ble_gatts_value_set(p_device_config_service->conn_handle,p_device_config_service->client_provision_char_handles.value_handle, &gatts_value);
			p_rw_authorize_reply_params.type = BLE_GATTS_AUTHORIZE_TYPE_READ;
			//NRF_LOG_INFO("Timeseries \n");
		}
		err_code = sd_ble_gatts_rw_authorize_reply(p_device_config_service->conn_handle,&p_rw_authorize_reply_params);
		
		return err_code;
}


/**@brief Function for handling the Connect event.
 *
 * @param[in]   p_device_config_service  DEVICE_CONFIG  service structure.
 * @param[in]   p_ble_evt      Event received from the BLE stack.
 */
static void on_connect(ble_device_config_service_t * p_device_config_service, ble_evt_t const * p_ble_evt)
{
    p_device_config_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Disconnect event.
 *
 * @param[in]   p_bas       DEVICE_CONFIG  service structure.
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_device_config_service_t * p_device_config_service, ble_evt_t const * p_ble_evt)
{
    UNUSED_PARAMETER(p_ble_evt);
    p_device_config_service->conn_handle = BLE_CONN_HANDLE_INVALID;
		p_device_config_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}

/**@brief Function for handling the Write event.
 *
 * @param[in] p_device_config_service   DEVICE_CONFIG  Service structure.
 * @param[in] p_ble_evt       Event received from the BLE stack.
 */
static void on_write(ble_device_config_service_t * p_device_config_service, ble_evt_t const * p_ble_evt)
{
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
		// Write protection
		if(!dfu.auth)
			return;
		// Device Provision Write
		if ((p_evt_write->handle == p_device_config_service->device_provision_char_handles.value_handle) && (p_device_config_service->device_provision_write_handler != NULL)){
				bool no_err = false;
				//uint32_t recv_rtr = 0;
				//no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &recv_rtr, (uint8_t *)"rtr");
				//if(recv_rtr == timeseries.rtr && no_err){
				if(1){
					// NRF_LOG_INFO("Received true");
					// "rtr": 65001, "tkn": 123456789, "ts": 1630586053, "lt": 1630586953
					uint32_t temp32_int = 0;
					uint8_t tempstr[50];
					uint8_t updates = 0;
					
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"did",p_evt_write->len);			// Device ID "did"
					if(no_err){		// Extract did
						if(dev.id != temp32_int && temp32_int>0){
							updates++;
						}
						dev.id = temp32_int;
						provision_state_handler(PROVISIONING_STATUS_SERIALIZED);
						NRF_LOG_INFO("Received did %d \n",dev.id);
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"psc",p_evt_write->len);			// Payg Token starting code "psc"
					if(no_err){		// Extract psc
						if(dev.start_code != temp32_int){
							updates++;
						}
						dev.start_code = temp32_int;
						NRF_LOG_INFO("Received psc %d \n",dev.start_code);
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &tempstr, (uint8_t *)"sat",p_evt_write->len);			// Server Auth Token "sat"
					if(no_err){		// Extract sat
						size_t n = sizeof(dev.server_auth_token)/sizeof(dev.server_auth_token[0]);
						if(strncmp((char*)dev.server_auth_token,(char*)tempstr,n) != 0){
							//Allow Reprovision && strcmp((char*)dev.server_auth_token,"")==0){
							updates++;
							provision_state_handler(PROVISIONING_STATUS_PROVISIONED);
						}
						strncpy((char *)dev.server_auth_token,(char *)tempstr,n);
						NRF_LOG_INFO("Received sat %s \n",dev.server_auth_token);

					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &tempstr, (uint8_t *)"dsc",p_evt_write->len);			// Device Secret "dsc"
					if(no_err){		// Extract dsc
						size_t n = sizeof(dev.secret)/sizeof(dev.secret[0]);
						if(memcmp(dev.secret,tempstr,n) != 0){
							updates++;
						}
						memcpy((char *)dev.secret,(char *)tempstr,n);
						static char hex_string[50];
						for(uint8_t i=0; i<n; i++){
							sprintf(hex_string+(2*i),"%02X",dev.secret[i]);
						}
						identity_init();
						//NRF_LOG_INFO("Received dsc %s \n",hex_string);
						NRF_LOG_DEBUG("Received dsc %s \n",hex_string);
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"pst",p_evt_write->len);			// Provisioning Status "pst"
					if(no_err){		// Extract pst
						if(dev.provisioning_status != temp32_int){
							updates++;
						}
						provision_state_handler(temp32_int);
						//dev.provisioning_status = temp32_int;
						NRF_LOG_INFO("Received pst %d \n",dev.provisioning_status);
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"opmax",p_evt_write->len);			// Productive Output Set Limit  "opmax"
					if(no_err){		// Extract opmax
						if(dev.productive_output_set_limit != temp32_int){
							updates++;
						}
						dev.productive_output_set_limit = temp32_int;
						NRF_LOG_INFO("Received opmax %d \n",dev.productive_output_set_limit);
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"cut",p_evt_write->len);			// time
					if(no_err){		// Extract cut
						//if(dev.productive_output_set_limit != temp32_int){
						//	updates++;
						//}
						uint32_t current_time = temp32_int;
						if(current_time>1647759912){
							//dev.time_calibration_factor = current_time/system_uptime;
							dev.time_calibration_factor[0] = 60; 																// Changes from minutes to seconds
							dev.time_calibration_factor[1] = current_time - (system_uptime*60);	// Y - Intercept
							NRF_LOG_INFO("Received cut:%zu fc:%zu\n",current_time,dev.time_calibration_factor);
						}
						else{
							NRF_LOG_INFO("BAD UNIX TIME:%zu\n",current_time);
						}
					}
										
					char data[] = {updates};
					p_device_config_service->device_provision_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_device_config_service, data);

				}
				else{
					NRF_LOG_INFO("Map error \n");
				}
		}
		// Client Provision Write
		if ((p_evt_write->handle == p_device_config_service->client_provision_char_handles.value_handle) && (p_device_config_service->client_provision_write_handler != NULL)){
				bool no_err = false;
				//uint32_t recv_rtr = 0;
				//no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &recv_rtr, (uint8_t *)"rtr");
				//if(recv_rtr == timeseries.rtr && no_err){
				if(1){
					// NRF_LOG_INFO("Received true");
					// "rtr": 65001, "tkn": 123456789, "ts": 1630586053, "lt": 1630586953
					uint32_t temp32_int = 0;
					uint8_t tempstr[50];
					uint8_t updates = 0;
					
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &tempstr, (uint8_t *)"cn",p_evt_write->len);				// Customer Name "cn"
					if(no_err){		// Extract cn
						size_t n = sizeof(cust.name)/sizeof(cust.name[0]);
						if(strncmp(cust.name,(char *)tempstr,n) != 0){
							updates++;
						}
						strncpy((char *)cust.name,(char *)tempstr,n);
						NRF_LOG_INFO("Received cn %s \n",cust.name);
						//updates++;
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &tempstr, (uint8_t *)"cp",p_evt_write->len);				// Customer's Phone "cp"
					if(no_err){		// Extract cp
						size_t n = sizeof(cust.phone)/sizeof(cust.phone[0]);
						if(strncmp(cust.phone,(char *)tempstr,n) != 0){
							updates++;
						}
						strncpy((char *)cust.phone,(char *)tempstr,n);
						NRF_LOG_INFO("Received cp %s \n",cust.phone);
						//updates++;
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"rid",p_evt_write->len);			// Readable ID "rid"
					if(no_err){		// Extract rid
						if(cust.readable_id != temp32_int){
							updates++;
						}
						cust.readable_id = temp32_int;
						NRF_LOG_INFO("Received rid %d \n",cust.readable_id);
						//updates++;
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &tempstr, (uint8_t *)"sat",p_evt_write->len);				// Server Auth Token "sat"
					if(no_err){		// Extract sat
						size_t n = sizeof(dev.server_auth_token)/sizeof(dev.server_auth_token[0]);
						if(strncmp((char*)dev.server_auth_token,(char*)tempstr,n) != 0){
							//Allow Reprovision && strcmp((char*)dev.server_auth_token,"")==0){
							updates++;
						}
						strncpy((char *)cust.server_auth_token,(char *)tempstr,n);
						provision_state_handler(PROVISIONING_STATUS_PROVISIONED);
						NRF_LOG_INFO("Received sat %s \n",dev.server_auth_token);					// cust.server_auth_token is referencing dev.server_auth_token
						//updates++;
					}
					no_err = byte_cborMap_decode_find((uint8_t *)p_evt_write->data, &temp32_int, (uint8_t *)"pst",p_evt_write->len);			// Provisioning Status "pst"
					if(no_err){		// Extract pst
						if(*(uint8_t *)cust.provisioning_status != temp32_int){
							updates++;
						}
						//*(uint8_t *)cust.provisioning_status = temp32_int;
						provision_state_handler(temp32_int);
						NRF_LOG_INFO("Received pst %d \n",dev.provisioning_status);				// cust.provisioning_status is referencing dev.provisioning_status
						//updates++;
					}
					char data[] = {updates};
					p_device_config_service->client_provision_write_handler(p_ble_evt->evt.gap_evt.conn_handle, p_device_config_service, data);
				}
				else{
					NRF_LOG_INFO("Map error \n");
				}
		}
}

/**@brief Function for adding the device_provision  characteristic.
 *
 */
static uint32_t device_provision_char_add(ble_device_config_service_t * p_device_config_service)
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
    char_md.p_char_user_desc         = Device_Provision_CharName;
    char_md.char_user_desc_size      = sizeof(Device_Provision_CharName);
    char_md.char_user_desc_max_size  = sizeof(Device_Provision_CharName);
    char_md.p_char_pf                = NULL;
    char_md.p_user_desc_md           = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;

    // Define the device_provision Characteristic UUID
    ble_uuid.type = p_device_config_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_DEVICE_PROVISION_CHAR_UUID;

    // Set permissions on the Characteristic value
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);

    // Attribute Metadata settings
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
		
		// Update attribute value
		update_device_provision_data_to_read();

		// Attribute Value settings
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = device_provision_data_to_read_index;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 100;//sizeof(uint8_t);
    attr_char_value.p_value      = device_provision_data_to_read;

    return sd_ble_gatts_characteristic_add(p_device_config_service->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_device_config_service->device_provision_char_handles);
}

static uint32_t client_provision_char_add(ble_device_config_service_t * p_device_config_service)
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
    char_md.p_char_user_desc         = Client_Provision_CharName;
    char_md.char_user_desc_size      = sizeof(Client_Provision_CharName);
    char_md.char_user_desc_max_size  = sizeof(Client_Provision_CharName);
    char_md.p_char_pf                = NULL;
    char_md.p_cccd_md                = NULL;
    char_md.p_sccd_md                = NULL;
		
		// Define the device_provision Characteristic UUID
    ble_uuid.type = p_device_config_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_CLIENT_PROVISION_CHAR_UUID;

    // Set permissions on the Characteristic value
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);

    // Attribute Metadata settings
    attr_md.vloc       = BLE_GATTS_VLOC_STACK;
    attr_md.rd_auth    = 1;
    attr_md.wr_auth    = 0;
    attr_md.vlen       = 1;
		
		// Update attribute value
		update_client_provision_data_to_read();

		// Attribute Value settings
    attr_char_value.p_uuid       = &ble_uuid;
    attr_char_value.p_attr_md    = &attr_md;
    attr_char_value.init_len     = client_provision_data_to_read_index;
    attr_char_value.init_offs    = 0;
    attr_char_value.max_len      = 100;//sizeof(uint8_t);
    attr_char_value.p_value      = client_provision_data_to_read;
		
    return sd_ble_gatts_characteristic_add(p_device_config_service->service_handle, &char_md,
                                           &attr_char_value,
                                           &p_device_config_service->client_provision_char_handles);
}

uint32_t ble_device_config_service_init(ble_device_config_service_t * p_device_config_service, const ble_device_config_service_init_t * p_device_config_service_init)
{
    uint32_t   err_code;
    ble_uuid_t ble_uuid;

    // Initialize service structure
    p_device_config_service->conn_handle = BLE_CONN_HANDLE_INVALID;

    // Initialize service structure.
    p_device_config_service->device_provision_write_handler = p_device_config_service_init->device_provision_write_handler;
		p_device_config_service->client_provision_write_handler = p_device_config_service_init->client_provision_write_handler;
	
    // Add service UUID
    ble_uuid128_t base_uuid = {BLE_UUID_DEVICE_CONFIG_SERVICE_BASE_UUID};
    err_code = sd_ble_uuid_vs_add(&base_uuid, &p_device_config_service->uuid_type);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Set up the UUID for the service (base + service-specific)
    ble_uuid.type = p_device_config_service->uuid_type;
    ble_uuid.uuid = BLE_UUID_DEVICE_CONFIG_SERVICE_UUID;

    // Set up and add the service
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY, &ble_uuid, &p_device_config_service->service_handle);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    // Add the different characteristics in the service:
    //   device_provision
    err_code = device_provision_char_add(p_device_config_service);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		//   client_provision
    err_code = client_provision_char_add(p_device_config_service);
    if (err_code != NRF_SUCCESS)
    {
        return err_code;
    }
		/*** Characteristic DFU can be moved here from ble_dfu_buttonless_init ***/
		// Add vendor specific base UUID to use with the Buttonless DFU characteristic.
    err_code = sd_ble_uuid_vs_add(&base_uuid, &m_dfu.uuid_type);
    VERIFY_SUCCESS(err_code);
    //Add the Buttonless DFU Characteristic (with bonds/without bonds).
		err_code = ble_dfu_buttonless_char_add(&m_dfu);
    VERIFY_SUCCESS(err_code);
		
		return NRF_SUCCESS;
}

void ble_device_config_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    ble_device_config_service_t * p_device_config_service = (ble_device_config_service_t *)p_context;

    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_device_config_service, p_ble_evt);
            break;

        case BLE_GATTS_EVT_WRITE:
            on_write(p_device_config_service, p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_device_config_service, p_ble_evt);
            break;
				
				case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
						ble_device_config_data_resp_update(p_device_config_service,p_ble_evt);
						break;

        default:
            // No implementation needed.
            break;
    }
		
}
