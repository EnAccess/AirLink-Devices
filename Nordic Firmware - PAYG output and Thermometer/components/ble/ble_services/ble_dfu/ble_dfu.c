/**
 * Copyright (c) 2017 - 2020, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/* Attention!
 *  To maintain compliance with Nordic Semiconductor ASA's Bluetooth profile
 *  qualification listings, this section of source code must not be modified.
 */

#include "ble_dfu.h"
#include <string.h>
#include "ble_hci.h"
#include "sdk_macros.h"
#include "ble_srv_common.h"
#include "nrf_nvic.h"
#include "nrf_sdm.h"
#include "nrf_soc.h"
#include "nrf_log.h"
#include "nrf_dfu_ble_svci_bond_sharing.h"
#include "nrf_bootloader_info.h"
#include "nrf_svci_async_function.h"
#include "nrf_pwr_mgmt.h"
#include "peer_manager.h"
#include "gatts_cache_manager.h"
#include "peer_id.h"
/*** This files are added to support resource model format ****/
#include "cbor_definitions.h"
#include "SEGGER_RTT.h"
#include "ssl_config.h"
	

#define MAX_CTRL_POINT_RESP_PARAM_LEN   3                           /**< Max length of the responses. */

#define BLE_DFU_SERVICE_UUID            0xFE59                      /**< The 16-bit UUID of the Secure DFU Service. */
/**** m_dfu moved to non static so that it can be shared with device_config_service ****/
//static ble_dfu_buttonless_t             m_dfu;                      /**< Structure holding information about the Buttonless Secure DFU Service. */

NRF_SDH_BLE_OBSERVER(m_dfus_obs, BLE_DFU_BLE_OBSERVER_PRIO, ble_dfu_buttonless_on_ble_evt, &m_dfu);

/**@brief Function that is called if no event handler is provided.
 */
static void dummy_evt_handler(ble_dfu_buttonless_evt_type_t evt)
{
    NRF_LOG_DEBUG("Dummy event handler received event 0x%x", evt);
}


/**@brief Function for handling write events to the Buttonless Secure DFU Service Service Control Point characteristic.
 *
 * @param[in]   p_evt_write   Write event received from the BLE stack.
 */
static void on_ctrlpt_write(ble_gatts_evt_write_t const * p_evt_write)
{
    uint32_t      err_code;
			
    ble_gatts_rw_authorize_reply_params_t write_authorize_reply;
    memset(&write_authorize_reply, 0, sizeof(write_authorize_reply));

    write_authorize_reply.type   = BLE_GATTS_AUTHORIZE_TYPE_WRITE;

    uint8_t cccd_val[2];
    ble_gatts_value_t value = {.p_value = cccd_val, .len = 2, .offset = 0};
    err_code = sd_ble_gatts_value_get(m_dfu.conn_handle, m_dfu.control_point_char.cccd_handle, &value);
    if (err_code == NRF_SUCCESS && ble_srv_is_indication_enabled(cccd_val))
    //if(1)	// We are bypassing checking of indication to allow airlink to work
		{
        write_authorize_reply.params.write.update      = 1;
        write_authorize_reply.params.write.gatt_status = BLE_GATT_STATUS_SUCCESS;
    }
    else
    {
        write_authorize_reply.params.write.gatt_status = BLE_GATT_STATUS_ATTERR_CPS_CCCD_CONFIG_ERROR;
    }
		
    // Authorize the write request
    do {
        err_code = sd_ble_gatts_rw_authorize_reply(m_dfu.conn_handle, &write_authorize_reply);
    } while (err_code == NRF_ERROR_BUSY);

		
    if (write_authorize_reply.params.write.gatt_status != BLE_GATT_STATUS_SUCCESS)
    {
        return;
    }
		
		// Forward the write event to the Buttonless DFU module.
    ble_dfu_buttonless_on_ctrl_pt_write(p_evt_write);
}


/**@brief Write authorization request event handler.
 *
 * @details The write authorization request event handler is called when writing to the control point.
 *
 * @param[in]   p_ble_evt Event received from the BLE stack.
 */
static void on_rw_authorize_req(ble_evt_t const * p_ble_evt)
{
		if (p_ble_evt->evt.gatts_evt.conn_handle != m_dfu.conn_handle)
    {
        return;
    }
	  const ble_gatts_evt_rw_authorize_request_t * p_auth_req =
        &p_ble_evt->evt.gatts_evt.params.authorize_request;
		if (
        (p_auth_req->type == BLE_GATTS_AUTHORIZE_TYPE_WRITE)                            &&
        (p_auth_req->request.write.handle == m_dfu.control_point_char.value_handle)     &&
        (p_auth_req->request.write.op != BLE_GATTS_OP_PREP_WRITE_REQ)                   &&
        (p_auth_req->request.write.op != BLE_GATTS_OP_EXEC_WRITE_REQ_NOW)               &&
        (p_auth_req->request.write.op != BLE_GATTS_OP_EXEC_WRITE_REQ_CANCEL)
       )
    {
      on_ctrlpt_write(&p_auth_req->request.write);
    }
}

uint32_t ble_dfu_buttonless_resp_update()
{
  	ble_gatts_value_t gatts_value;
		uint8_t value[100];
    gatts_value.len = update_dfu_data_to_read(value);
    gatts_value.offset  = 0;
    gatts_value.p_value = value;  // 0x0001 (Notification).
		
    uint32_t                err_code;
		err_code = sd_ble_gatts_value_set(BLE_CONN_HANDLE_INVALID,m_dfu.control_point_char.value_handle, &gatts_value);//sd_ble_gatts_hvx(m_dfu.conn_handle, &hvx_params);

		return err_code;
}


/**@brief Connect event handler.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_connect(ble_evt_t const * p_ble_evt)
{
    m_dfu.conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}


/**@brief Disconnect event handler.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 */
static void on_disconnect(ble_evt_t const * p_ble_evt)
{
    if (m_dfu.conn_handle != p_ble_evt->evt.gap_evt.conn_handle)
    {
        return;
    }
		ble_dfu_buttonless_resp_update();
		m_dfu.conn_handle = BLE_CONN_HANDLE_INVALID;
		
}


/**@brief Function for handling the HVC events.
 *
 * @details Handles HVC events from the BLE stack.
 *
 * @param[in] p_ble_evt  Event received from the BLE stack.
 */
static void on_hvc(ble_evt_t const * p_ble_evt)
{
    uint32_t err_code;
    ble_gatts_evt_hvc_t const * p_hvc = &p_ble_evt->evt.gatts_evt.params.hvc;

    if (p_hvc->handle == m_dfu.control_point_char.value_handle)
    {
        // Enter bootloader if we were waiting for reset after hvc indication confimation.
        if (m_dfu.is_waiting_for_reset)
        {
            err_code = ble_dfu_buttonless_bootloader_start_prepare();
            if (err_code != NRF_SUCCESS)
            {
                m_dfu.evt_handler(BLE_DFU_EVT_BOOTLOADER_ENTER_FAILED);
            }
        }
    }
}


void ble_dfu_buttonless_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
    VERIFY_PARAM_NOT_NULL_VOID(p_ble_evt);
		
    switch (p_ble_evt->header.evt_id)
    {
        case BLE_GAP_EVT_CONNECTED:
            on_connect(p_ble_evt);
            break;

        case BLE_GAP_EVT_DISCONNECTED:
            on_disconnect(p_ble_evt);
            break;

        case BLE_GATTS_EVT_RW_AUTHORIZE_REQUEST:
            on_rw_authorize_req(p_ble_evt);
            break;

        case BLE_GATTS_EVT_HVC:
            on_hvc(p_ble_evt);
            break;

        default:
            // no implementation
            break;
    }
}


uint32_t ble_dfu_buttonless_resp_send(ble_dfu_buttonless_op_code_t op_code, ble_dfu_buttonless_rsp_code_t rsp_code)
{
    // Send indication
    uint32_t                err_code;
    const uint16_t          len = MAX_CTRL_POINT_RESP_PARAM_LEN;
    uint16_t                hvx_len;
    uint8_t                 hvx_data[MAX_CTRL_POINT_RESP_PARAM_LEN];
    ble_gatts_hvx_params_t  hvx_params;

    memset(&hvx_params, 0, sizeof(hvx_params));

    hvx_len     = len;
    hvx_data[0] = DFU_OP_RESPONSE_CODE;
    hvx_data[1] = (uint8_t)op_code;
    hvx_data[2] = (uint8_t)rsp_code;

    hvx_params.handle = m_dfu.control_point_char.value_handle;
		
		// Bypassing indication with cbor encoded data for not dfu commands
		if(rsp_code<0xA1){
			hvx_params.type   = BLE_GATT_HVX_INDICATION;
			hvx_params.offset = 0;
			hvx_params.p_len  = &hvx_len;
			hvx_params.p_data = hvx_data;
		}
		else{
			uint8_t value[100];
			hvx_len = update_dfu_data_to_read(value);
    	hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
			hvx_params.offset = 0;
			hvx_params.p_len  = &hvx_len;
			hvx_params.p_data = value;
		}
			
    err_code = sd_ble_gatts_hvx(m_dfu.conn_handle, &hvx_params);
    if ((err_code == NRF_SUCCESS) && (hvx_len != len))
    {
        err_code = NRF_ERROR_DATA_SIZE;
    }

    return err_code;
}


uint32_t ble_dfu_buttonless_bootloader_start_finalize(void)
{
    uint32_t err_code;

    NRF_LOG_DEBUG("In ble_dfu_buttonless_bootloader_start_finalize\r\n");

    err_code = sd_power_gpregret_clr(0, 0xffffffff);
    VERIFY_SUCCESS(err_code);

    err_code = sd_power_gpregret_set(0, BOOTLOADER_DFU_START);
    VERIFY_SUCCESS(err_code);

    // Indicate that the Secure DFU bootloader will be entered
    m_dfu.evt_handler(BLE_DFU_EVT_BOOTLOADER_ENTER);

    // Signal that DFU mode is to be enter to the power management module
    nrf_pwr_mgmt_shutdown(NRF_PWR_MGMT_SHUTDOWN_GOTO_DFU);

    return NRF_SUCCESS;
}


uint32_t ble_dfu_buttonless_init(const ble_dfu_buttonless_init_t * p_dfu_init)
{
    uint32_t        err_code;
    
    VERIFY_PARAM_NOT_NULL(p_dfu_init);
		// Initialize the service structure.
    m_dfu.conn_handle                  = BLE_CONN_HANDLE_INVALID;
    m_dfu.evt_handler                  = p_dfu_init->evt_handler;
    m_dfu.is_waiting_for_reset         = false;

    if (m_dfu.evt_handler == NULL)
    {
        m_dfu.evt_handler = dummy_evt_handler;
    }

    err_code = ble_dfu_buttonless_backend_init(&m_dfu);
    VERIFY_SUCCESS(err_code);
    /*** Characteristic DFU can be moved to ble_device_config_service_init ***/
		/* There is no need for this service and characteristics as duplicate characteristics is in ble_device_config_service
    ble_uuid_t      service_uuid;
    ble_uuid128_t   nordic_base_uuid = BLE_UUID_SSL_BASE_UUID;//BLE_NORDIC_VENDOR_BASE_UUID;
		
		BLE_UUID_BLE_ASSIGN(service_uuid, BLE_DFU_SERVICE_UUID);

    // Add the DFU service declaration.
    err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &(m_dfu.service_handle));

    VERIFY_SUCCESS(err_code);

    // Add vendor specific base UUID to use with the Buttonless DFU characteristic.
    err_code = sd_ble_uuid_vs_add(&nordic_base_uuid, &m_dfu.uuid_type);
    VERIFY_SUCCESS(err_code);
    // Add the Buttonless DFU Characteristic (with bonds/without bonds).
    err_code = ble_dfu_buttonless_char_add(&m_dfu);
    VERIFY_SUCCESS(err_code);
		*/// dfu char end
    return NRF_SUCCESS;
}

/**@brief Helper Function for preparation of dfu data to read from BLE
 * @param[in]   None.
 * @param[out]  None.
 */
uint8_t update_dfu_data_to_read(uint8_t * data_to_read){
	  uint8_t dfu_data_to_read_index = 0;
		
		// Add cbor header of map with 5 elements 
		dfu_data_to_read_index = byte_cborArr_encode(data_to_read,dfu_data_to_read_index,5,false);
		// 1. Add cbor formated dfu_resource_rtr (2+1 Byte)
		dfu_data_to_read_index = byte_cborStr_encode(data_to_read,dfu_data_to_read_index, (uint8_t *)"rtr",strlen("rtr"),true);
		dfu_data_to_read_index = byte_cbor16_encode(data_to_read,dfu_data_to_read_index, dfu.rtr);
	
		// 2. Add cbor formated AirLink dfu resource version "rv" (2+1 Byte)
		dfu_data_to_read_index = byte_cborStr_encode(data_to_read,dfu_data_to_read_index, (uint8_t *)"rv",strlen("rv"),true);
		dfu_data_to_read_index = byte_cbor32_encode(data_to_read,dfu_data_to_read_index, dfu.resource_version);
		// 3. Add cbor formated AirLink FW Version "fv" (4+1 Byte)
		dfu_data_to_read_index = byte_cborStr_encode(data_to_read,dfu_data_to_read_index, (uint8_t *)"fv",strlen("fv"),true);
		dfu_data_to_read_index = byte_cbor16_encode(data_to_read,dfu_data_to_read_index, dfu.fw_version);
		// 4. Add cbor formated AirLink File size accepted? "fs" (?+1 Byte)
		dfu_data_to_read_index = byte_cborStr_encode(data_to_read,dfu_data_to_read_index, (uint8_t *)"fs",strlen("fs"),true);
		dfu_data_to_read_index = byte_cbor32_encode(data_to_read,dfu_data_to_read_index, dfu.file_size);
		// 5. Add cbor formated HW version? "hv" (1+1 Byte)
		dfu_data_to_read_index = byte_cborStr_encode(data_to_read,dfu_data_to_read_index, (uint8_t *)"hv",strlen("hv"),true);
		dfu_data_to_read_index = byte_cbor16_encode(data_to_read,dfu_data_to_read_index, dfu.hw_version);
		
		#ifdef DEBUG 
			SEGGER_RTT_printf(0,"dfu:");
			for(int i=0;i<dfu_data_to_read_index;i++){
				SEGGER_RTT_printf(0,"%02x ",data_to_read[i]);
			}
			SEGGER_RTT_printf(0,"\r\n");
		#endif
		return dfu_data_to_read_index;
}
