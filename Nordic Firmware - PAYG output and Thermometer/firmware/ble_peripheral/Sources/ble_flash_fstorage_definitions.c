/**
 * Copyright (c) 2016 - 2020, Nordic Semiconductor ASA
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

/** @file
 *
 * @brief fstorage example main file.
 *
 * This example showcases fstorage usage.
 */
 
 #include "ble_flash_fstorage_definitions.h"
 #include "ble_pwr_mgmt_definitions.h"
 #include "ssl_config.h"
 #include "nx_common.h"
 #include "src/nexus_nv.h"
 #include "include/nxp_common.h"
 #include "SEGGER_RTT.h"           
 #include "payg_state.h"

 nrf_fstorage_api_t * p_fs_api;
 //char data_read[256] = {'h','e','l','l','0',0};
 uint32_t error_code = 0xABAD5EED;
 const uint32_t read_mark = 0x77;
	 
 const uint32_t page1_start_address 	= 0x6C000;			// Configuration
 const uint32_t page1_end_address 		= 0x6CFFF;			 
 const uint32_t page2_start_address 	= 0x6B000;			// System Time (Uptime and payg credit remaining)
 const uint32_t page2_end_address 		= 0x6BFFF;
 const uint32_t page3_start_address 	= 0x6A000;			// Nexus Keycode
 const uint32_t page3_end_address 		= 0x6AFFF;
 const uint32_t page4_start_address 	= 0x69000;			// PAYG
 const uint32_t page4_end_address 		= 0x69FFF;			
 const uint32_t page5_start_address 	= 0x68000;			// RESERVED
 const uint32_t page5_end_address 		= 0x68FFF;
 const uint32_t page6_start_address 	= 0x67000;			// RESERVED
 const uint32_t page6_end_address 		= 0x67FFF;
 const uint32_t page7_start_address 	= 0x66000;			// RESERVED
 const uint32_t page7_end_address 		= 0x66FFF;
 const uint32_t page8_start_address 	= 0x65000;			// RESERVED
 const uint32_t page8_end_address 		= 0x65FFF;
 const uint32_t page9_start_address 	= 0x64000;			// TEMP
 const uint32_t page9_end_address 		= 0x64FFF;
 const uint32_t page10_start_address 	= 0x63000;			// RESERVED
 const uint32_t page10_end_address 		= 0x63FFF;
 const uint32_t page11_start_address 	= 0x62000;			// RESERVED
 const uint32_t page11_end_address 		= 0x62FFF;
 
 const uint16_t page_size = 4096;
 const uint16_t block_size = 512; 
 
// These constants correspond to position of item in arrays: ssl_block_size[] and ssl_block_per_page[]
// Dont Change
const uint8_t CONFIG_PAGE = 1;
const uint8_t SYSTEM_TIME_PAGE = 2;
const uint8_t NEXUS_PAGE = 3;
const uint8_t PAYG_PAGE = 4;
const uint8_t TEMP_PAGE = 9;
const uint8_t LAST_PAGE = 11;

const uint16_t CONFIG_PAGE_FLAG 			= 0x0001;
const uint16_t SYSTEM_TIME_PAGE_FLAG 	= 0x0002;
const uint16_t NEXUS_PAGE_FLAG 				= 0x0004;
const uint16_t PAYG_PAGE_FLAG 				= 0x0008;
const uint16_t TEMP_PAGE_FLAG 				= 0x0100;
const uint16_t TIMESERIES_FLAG 				= 0x4000;
const uint16_t MARKER_FLAG						= 0x8000;

 
 uint8_t read_index = 255;
 uint32_t marker_address=0, marker_index=0;
 
 NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
	{
			/* Set a handler for fstorage events. */
			.evt_handler = fstorage_evt_handler,

			/* These below are the boundaries of the flash space assigned to this instance of fstorage.
			 * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
			 * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
			 * last page of flash available to write data. */
			.start_addr = page11_start_address,
			.end_addr   = page1_end_address,
	};

 /**@brief   Helper function to obtain the last address on the last page of the on-chip flash that
  *          can be used to write user data.
  */
 uint32_t nrf5_flash_end_addr_get()
 {
    uint32_t const bootloader_addr = BOOTLOADER_ADDRESS;
    uint32_t const page_sz         = NRF_FICR->CODEPAGESIZE;
    uint32_t const code_sz         = NRF_FICR->CODESIZE;

    return (bootloader_addr != 0xFFFFFFFF ?
            bootloader_addr : (code_sz * page_sz));
 }

 static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
 {
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation.");
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        default:
            break;
    }
 }


 void print_flash_info(nrf_fstorage_t * p_fstorage)
 {
    NRF_LOG_INFO("========| flash info |========");
    NRF_LOG_INFO("erase unit: \t%d bytes",      p_fstorage->p_flash_info->erase_unit);
    NRF_LOG_INFO("program unit: \t%d bytes",    p_fstorage->p_flash_info->program_unit);
    NRF_LOG_INFO("==============================");
 }


 void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
 {
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage))
    {
        idle_state_handle();
    }
 }

 void flash_init(void){
#ifdef SOFTDEVICE_PRESENT
    ret_code_t rc; 
	  NRF_LOG_INFO("SoftDevice is present.");
    NRF_LOG_INFO("Initializing nrf_fstorage_sd implementation...");
    /* Initialize an fstorage instance using the nrf_fstorage_sd backend.
     * nrf_fstorage_sd uses the SoftDevice to write to flash. This implementation can safely be
     * used whenever there is a SoftDevice, regardless of its status (enabled/disabled). */
    p_fs_api = &nrf_fstorage_sd;
#else
    NRF_LOG_INFO("SoftDevice not present.");
    NRF_LOG_INFO("Initializing nrf_fstorage_nvmc implementation...");
    /* Initialize an fstorage instance using the nrf_fstorage_nvmc backend.
     * nrf_fstorage_nvmc uses the NVMC peripheral. This implementation can be used when the
     * SoftDevice is disabled or not present.
     *
     * Using this implementation when the SoftDevice is enabled results in a hardfault. */
    p_fs_api = &nrf_fstorage_nvmc;
#endif
		rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
		APP_ERROR_CHECK(rc);

	  print_flash_info(&fstorage);

	  /* It is possible to set the start and end addresses of an fstorage instance at runtime.
		 * They can be set multiple times, should it be needed. The helper function below can
		 * be used to determine the last address on the last page of flash memory available to
		 * store data. */
	  (void) nrf5_flash_end_addr_get();
    //NRF_LOG_INFO("Use 'read' to read bytes from the flash.");
    //NRF_LOG_INFO("Use 'write' to write bytes to the flash.");
    //NRF_LOG_INFO("Use 'erase' to erase flash pages.");
    //NRF_LOG_INFO("Use 'flasharea' to print and configure the flash read boundaries.");
 
 }

 /**@brief   Helper function to execute flash write operation
  *          
  */
 void ssl_flash_update(){
		if(!flash_upload_flag)
			return;
		
		if(flash_upload_flag & CONFIG_PAGE_FLAG)			// Configuration Update
			ssl_low_fstorage_write(CONFIG_PAGE);
		if(flash_upload_flag & SYSTEM_TIME_PAGE_FLAG)	// Time Update
			ssl_low_fstorage_write(SYSTEM_TIME_PAGE);
		if(flash_upload_flag & NEXUS_PAGE_FLAG)				// Nexus Keycode Update
			ssl_low_fstorage_write(NEXUS_PAGE);
		if(flash_upload_flag & PAYG_PAGE_FLAG)				// PAYG Update
			ssl_low_fstorage_write(PAYG_PAGE);
		if(flash_upload_flag & TEMP_PAGE_FLAG)				// TEMP Update
			ssl_low_fstorage_write(TEMP_PAGE);		
		
		if(flash_upload_flag & TIMESERIES_FLAG){				// Timeseries Update
			flash_upload_flag |= TEMP_PAGE_FLAG;
			flash_upload_flag &= ~TIMESERIES_FLAG;
		}
		if(flash_upload_flag & MARKER_FLAG)						// MARKER Update
			timeseries_save_marker();
 }

  /**@brief   Helper function to load data from flash, ideal just after boot
  *          
  */
void ssl_flash_load(void){
		ssl_low_fstorage_load();
} 

 /**@brief   Helper function to bookmark addresses to be written for intergers
  *          
  */
static uint16_t byte_flash32_add(uint8_t * destination, uint16_t start_index, uint32_t value){
    
	destination[start_index++] = value & 0xff;					// Insert the LSB number;
	destination[start_index++] = (value>>8) & 0xff;			// Insert the LSB number;
	destination[start_index++] = (value>>16) & 0xff;    // Insert the MSB number;
	destination[start_index++] = (value>>24) & 0xff;		// Insert the MSB number;
	
	return start_index;
}

 /**@brief   Helper function to bookmark addresses to be written for string
  *          
  */
static uint16_t byte_flashStr_add(uint8_t * destination, uint16_t start_index, uint8_t * value, uint16_t length){
	 for(uint32_t i=0; i<length; i++){
	    destination[start_index+i] = value[i];
	 }
	 return start_index+length;
} 

 /**@brief   Helper function to read string from flash
  *          
  */
static uint16_t byte_flashStr_read(uint8_t * destination, uint32_t address, uint16_t start_index, uint16_t length){
	ret_code_t rc;
	rc = nrf_fstorage_read(&fstorage, address+start_index, destination, length);
	APP_ERROR_CHECK(rc);
	wait_for_flash_ready(&fstorage);
	return start_index + length;
}

 /**@brief   Helper function to read intergers from flash
  *          
  */
static uint16_t byte_flash32_read(void * destination, uint32_t address, uint16_t start_index,uint8_t size){
	uint32_t temp32=0;
	ret_code_t rc = nrf_fstorage_read(&fstorage, address+start_index, &temp32, 4);
	APP_ERROR_CHECK(rc);
	wait_for_flash_ready(&fstorage);
	if(size == 1){
		*(uint8_t *)destination = temp32 & 0xff;
		//NRF_LOG_INFO("FLASH8: %d\r\n",temp32);
	}
	else if(size == 2){
		*(uint16_t *)destination = temp32 & 0xffff;
		//NRF_LOG_INFO("FLASH16: %d\r\n",temp32);
	}
	else{
		*(uint32_t *)destination = temp32;
		//NRF_LOG_INFO("FLASH32: %d\r\n",temp32);
	}
	//NRF_LOG_INFO("size: %d\r\n",size);
	return start_index + 4;
}
/************************** LOW LEVEL FUNCTIONS EXTRA CARE IS NEEDED **************************/

/**@brief To utilize flash we need 3 things (Constants), Flash start address, Size of block and number of blocks in that one page
 * @detail Flash start address, this is the start address of page
 * @detail Size of block, one block is like one row of data, size of block is the sum of individual size each variable take in flash 
 * @detail number of blocks in that one page calculated as page size/ size of block
 */ 

const uint32_t page_start_address[] = {page1_start_address, page2_start_address, page3_start_address, page4_start_address, page5_start_address, \
																			 page6_start_address, page7_start_address, page8_start_address, page9_start_address, page10_start_address, \
																			 page11_start_address};

#if NEXUS_CHANNEL_LINK_SECURITY_ENABLED
	const uint16_t nexus_block_size = NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH+
																		NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+NX_COMMON_NV_BLOCK_6_LENGTH+
																		NX_COMMON_NV_BLOCK_7_LENGTH+NX_COMMON_NV_BLOCK_8_LENGTH+NX_COMMON_NV_BLOCK_9_LENGTH+
																		NX_COMMON_NV_BLOCK_10_LENGTH+NX_COMMON_NV_BLOCK_11_LENGTH+NX_COMMON_NV_BLOCK_12_LENGTH+
																		NX_COMMON_NV_BLOCK_13_LENGTH+4;
	const uint8_t nexus_block_per_page = 9; 	// page_size/(446+4)
#else
	const uint16_t nexus_block_size = NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+4;
	const uint8_t nexus_block_per_page = 146;	// page_size/(24+4)
#endif


/**@brief This structure hold indexes for configuration data
 */
struct configuration_page_indexes{
	uint16_t flash_guard;
	uint16_t cust_name;
	uint16_t cust_phone;
	uint16_t cust_readable_id;
	uint16_t cust_provisioning_id;
	uint16_t dev_id;
	uint16_t dev_start_code;
	uint16_t dev_provisioning_status;
	uint16_t dev_server_auth_token;
	uint16_t battery_device_alert_threshold;
	//uint16_t dummy;
	uint16_t dev_qa_flag;
	uint16_t payg_is_unlocked;
	uint16_t dev_secret;
	uint16_t dev_productive_output_set_limit;
	uint16_t temp_threshold_temperature;
	uint16_t proximity_lock;
};
static struct configuration_page_indexes conf_page_index;

/**@brief This structure hold indexes for payg data
 */
struct payg_page_indexes{
	uint16_t flash_guard;
	uint16_t payg_payG_credit_remaining;
	uint16_t payg_mode;
	uint16_t payg_last_payg_credit;
	uint16_t payg_last_payG_calc;
	uint16_t payg_last_payG_update;
	uint16_t payg_token;
	//uint16_t payg_local_time;
	uint16_t timeseries_index;
	uint16_t timeseries_read_marker;
};
static struct payg_page_indexes payg_page_index;

struct temp_page_indexes{
	uint16_t flash_guard;
	uint16_t temp_max_temperature;
	uint16_t temp_current_temperature;
	uint16_t timeseries_index;
	uint16_t timeseries_read_marker; //last read
};
static struct temp_page_indexes temp_page_index;

/**@brief This structure hold indexes for system time data
 */
struct system_time_page_indexes{
	uint16_t flash_guard;
	uint16_t payg_payG_credit_remaining;
	uint16_t system_uptime;
	uint16_t unix_time;
	uint16_t proximity_lock_counter;
};
static struct system_time_page_indexes system_time_page_index;


/**@brief Function for preparing data and address indexes for flash
 * @details 
 * @param[in] data_ptr container for data tobe written to flash, for reading is not used or can be void pointer
 * @param[in] mode = true prepare for write operation (data and indexes), false prepare for read operation (Indexes only)
 */
static void config_page_flash_data_prepare(uint8_t * data_ptr, bool mode){
	uint16_t x=0;
	char *ptr;
	ptr = (char*)malloc(block_size);
	
	//1. write flag (1 of 4 Bytes)
	conf_page_index.flash_guard = x;
	x = byte_flash32_add((uint8_t *) ptr, x,flash_guard);
	
	//2. write owner's name (16 of 16 Bytes)
	conf_page_index.cust_name = x;
	x = byte_flashStr_add((uint8_t *) ptr, x,(uint8_t *)cust.name, 16);
	//3. write owner's phone (16 of 16 Bytes)
	conf_page_index.cust_phone = x;
	x = byte_flashStr_add((uint8_t *) ptr, x,(uint8_t *)cust.phone, 16);

	//4. write readble ID (4 of 4 Bytes)
	conf_page_index.cust_readable_id = x;
	x = byte_flash32_add((uint8_t *) ptr, x, cust.readable_id);
	//5. write provisioning ID (1 of 4 Bytes)
	conf_page_index.cust_provisioning_id = x;
	x = byte_flash32_add((uint8_t *) ptr, x, (uint32_t)cust.provisioning_id);
	//6. write device ID (1 of 4 Bytes)
	conf_page_index.dev_id = x;
	x = byte_flash32_add((uint8_t *) ptr, x, dev.id);
	//7. write start_code  (1 of 4 Bytes)
	conf_page_index.dev_start_code = x;
	x = byte_flash32_add((uint8_t *) ptr, x, dev.start_code);
	//8. write provisioning_status (1 of 4 Bytes)
	conf_page_index.dev_provisioning_status = x;
	x = byte_flash32_add((uint8_t *) ptr, x, (uint32_t)dev.provisioning_status);
	//9. write server_auth_token (24 of 24 Bytes)
	conf_page_index.dev_server_auth_token = x;
	x = byte_flashStr_add((uint8_t *) ptr, x,dev.server_auth_token, 24);
	uint32_t dummy = 10; 	
	//10. write dummy (1 of 4 Bytes)
	conf_page_index.battery_device_alert_threshold = x;
	x = byte_flash32_add((uint8_t *) ptr, x,dummy++);
	//11. write dummy+1 (4 of 4 Bytes)
	conf_page_index.dev_qa_flag = x;
	x = byte_flash32_add((uint8_t *) ptr, x,dummy);
	//12. write is_locked (2 of 4 Bytes)
	conf_page_index.payg_is_unlocked = x;
	x = byte_flash32_add((uint8_t *) ptr, x,payg.is_unlocked);
	//13. write secret (16 of 16 Bytes)
	conf_page_index.dev_secret = x;
	x = byte_flashStr_add((uint8_t *) ptr, x,dev.secret, 16);
	//14. write opmax (4 of 4 Bytes)
	conf_page_index.dev_productive_output_set_limit = x;
	x = byte_flash32_add((uint8_t *) ptr, x,dev.productive_output_set_limit);
	//15. write threshold_temperature (4 of 4 Bytes)
	conf_page_index.temp_threshold_temperature = x;
	x = byte_flash32_add((uint8_t *) ptr, x,temp.threshold_temperature);	
	//16. write proximity_lock (4 of 4 Bytes)
	conf_page_index.proximity_lock = x;
	x = byte_flash32_add((uint8_t *) ptr, x,dev.proximity_lock);	
	
	if(mode)
	 memcpy(data_ptr,ptr,x);
	
	free(ptr);
}


/**@brief Function for preparing data and address indexes for flash
 * @details 
 * @param[in] data_ptr container for data tobe written to flash, for reading is not used or can be void pointer
 * @param[in] mode = true prepare for write operation (data and indexes), false prepare for read operation (Indexes only)
 */

static void payg_page_flash_data_prepare(uint8_t * data_ptr, bool mode){
	uint16_t x=0;
	char *ptr;
	
	ptr = (char*)malloc(block_size);
		
	//1. write flag (4 of 4 Bytes)
	payg_page_index.flash_guard = x;
	x = byte_flash32_add((uint8_t *) ptr, x,flash_guard);
	
	//2. write payG_credit_remaining (4 of 4 Bytes)
	//payg.payG_credit_remaining = payg_state_get_remaining_credit();
  payg_page_index.payg_payG_credit_remaining = x;
	x = byte_flash32_add((uint8_t *) ptr, x,payg.payG_credit_remaining);
	
	//3. write mode (1 of 4 Bytes)
	payg_page_index.payg_mode = x;
	x = byte_flash32_add((uint8_t *) ptr, x,(uint32_t)payg.mode);
	//4. write last_payg_credit (4 of 4 Bytes)
	payg_page_index.payg_last_payg_credit = x;
	x = byte_flash32_add((uint8_t *) ptr, x,payg.last_payg_credit);
	//5. write last_payG_calc (4 of 4 Bytes)
	payg_page_index.payg_last_payG_calc = x;
	x = byte_flash32_add((uint8_t *) ptr, x,payg.last_payG_calc);
	//6. write last_payG_update (4 of 4 Bytes)
	payg_page_index.payg_last_payG_update = x;
	x = byte_flash32_add((uint8_t *) ptr, x,payg.last_payG_update);
	//7. write Token (20 of 20 Bytes)
	payg_page_index.payg_token = x;
	x = byte_flashStr_add((uint8_t *) ptr, x,(uint8_t *)payg.token, 20);
	//x-1. write system_uptime (4 of 4 Bytes)
	payg_page_index.timeseries_index = x;
	x = byte_flash32_add((uint8_t *) ptr, x,system_uptime);
	//x. write timeseries_read_marker (4 of 4 Bytes)
	payg_page_index.timeseries_read_marker = x;
	x = byte_flash32_add((uint8_t *) ptr, x,0xFFFFFFFF);
	
	
	if(mode){
		memcpy(data_ptr,ptr,x);
		//NRF_LOG_DEBUG("system_uptime:%d payg.payG_credit_remaining:%d.",system_uptime,payg.payG_credit_remaining);
	}
	free(ptr);
}

static void temp_page_flash_data_prepare(uint8_t * data_ptr, bool mode){
	uint16_t x=0;
	char *ptr;
	
	ptr = (char*)malloc(block_size);
		
	//1. write flag (4 of 4 Bytes)
	temp_page_index.flash_guard = x;
	x = byte_flash32_add((uint8_t *) ptr, x,flash_guard);
	
	//2. write max_temperature (4 of 4 Bytes)
	temp_page_index.temp_max_temperature = x;
	x = byte_flash32_add((uint8_t *) ptr, x,temp.max_temperature);
	//3. write current_temperature (4 of 4 Bytes)
	temp_page_index.temp_current_temperature = x;
	x = byte_flash32_add((uint8_t *) ptr, x,temp.current_temperature);
	//x-1. write system_uptime (4 of 4 Bytes)
	temp_page_index.timeseries_index = x;
	x = byte_flash32_add((uint8_t *) ptr, x,system_uptime);
	//x. write timeseries_read_marker (4 of 4 Bytes)
	temp_page_index.timeseries_read_marker = x;
	x = byte_flash32_add((uint8_t *) ptr, x,0xFFFFFFFF);
	
	if(mode){
		memcpy(data_ptr,ptr,x);
		//NRF_LOG_DEBUG("system_uptime:%d payg.payG_credit_remaining:%d.",system_uptime,payg.payG_credit_remaining);
	}
	free(ptr);
}


/**@brief Function for preparing data and address indexes for flash
 * @details 
 * @param[in] data_ptr container for data tobe written to flash, for reading is not used or can be void pointer
 * @param[in] mode = true prepare for write operation (data and indexes), false prepare for read operation (Indexes only)
 */
static void system_time_page_flash_data_prepare(uint8_t * data_ptr, bool mode){
	uint16_t x=0;
	char *ptr;
	
	ptr = (char*)malloc(block_size);
		
	//1. write flag (4 of 4 Bytes)
	system_time_page_index.flash_guard = x;
	x = byte_flash32_add((uint8_t *) ptr, x,flash_guard);
	
	//2. write payG_credit_remaining (4 of 4 Bytes)
	payg.payG_credit_remaining = payg_state_get_remaining_credit();
	system_time_page_index.payg_payG_credit_remaining = x;
	x = byte_flash32_add((uint8_t *) ptr, x,payg.payG_credit_remaining);
	//3. write system_uptime (4 of 4 Bytes)
	system_time_page_index.system_uptime = x;
	x = byte_flash32_add((uint8_t *) ptr, x,system_uptime);
	//4. write proximity_lock_counter (4 of 4 Bytes)
	system_time_page_index.proximity_lock_counter = x;
	x = byte_flash32_add((uint8_t *) ptr, x,dev.proximity_lock_counter);
	//5. write dummy (4 of 4 Bytes)
	system_time_page_index.unix_time = x;
	x = byte_flash32_add((uint8_t *) ptr, x,0);
	
	if(mode){
		memcpy(data_ptr,ptr,x);
		//NRF_LOG_DEBUG("system_uptime:%d payg.payG_credit_remaining:%d.",system_uptime,payg.payG_credit_remaining);
	}
	free(ptr);
}

/**@brief Function for writing the data into the flash
 * @details 
 * @param[in] page_number, the flash page to write data into.
 */
static void ssl_low_fstorage_write(uint8_t page_number){
		ret_code_t rc;
		uint32_t start_address=0;
		
		page_number -= 1;
		// If page is not 1 or 2 or 3 or 4 or 5 do nothing, if there is flash wrong allocation do nothing
		if(page_number > (LAST_PAGE-1)){
			return;
		}
		
		if(start_address == 0){										// page is full erase and start from the start of page
			rc = nrf_fstorage_erase(&fstorage, page_start_address[page_number], 1, NULL);
			APP_ERROR_CHECK(rc);
			wait_for_flash_ready(&fstorage);
			NRF_LOG_INFO("Erase Done %x.",page_start_address[page_number]);
			start_address = page_start_address[page_number];
		}	
		
		if(page_number == CONFIG_PAGE-1){
			// Prepare string to be written to flash
			uint8_t *page0_ptr;
			page0_ptr = (uint8_t *)malloc(block_size);//ssl_block_size[page_number]*sizeof(char));
			
			config_page_flash_data_prepare(page0_ptr, true);
		
			rc = nrf_fstorage_write(&fstorage, start_address, page0_ptr, block_size, NULL);//ssl_block_size[page_number], NULL);
			APP_ERROR_CHECK(rc);
			wait_for_flash_ready(&fstorage);
			
			
			free(page0_ptr);
			flash_upload_flag &= ~CONFIG_PAGE_FLAG;				
		}
		else if(page_number == NEXUS_PAGE-1){
			flash_upload_flag &= ~NEXUS_PAGE_FLAG;
			return;
		}
		else if(page_number == SYSTEM_TIME_PAGE-1){
			// Prepare string to be written to flash
			uint8_t *page4_ptr;
			page4_ptr = (uint8_t *)malloc(block_size);//ssl_block_size[page_number]*sizeof(char));
		
			system_time_page_flash_data_prepare(page4_ptr, true);
			
			rc = nrf_fstorage_write(&fstorage, start_address, page4_ptr, block_size, NULL);//ssl_block_size[page_number], NULL);
			APP_ERROR_CHECK(rc);
			wait_for_flash_ready(&fstorage);
			free(page4_ptr);
		  flash_upload_flag &= ~SYSTEM_TIME_PAGE_FLAG;
		}
		else{
			uint8_t *page_ptr;
			page_ptr = (uint8_t *)malloc(block_size);//ssl_block_size[page_number]*sizeof(char));
			
			if(page_number == PAYG_PAGE-1){	
				payg_page_flash_data_prepare(page_ptr, true);
				flash_upload_flag &= ~PAYG_PAGE_FLAG;
			}
			else if(page_number == TEMP_PAGE-1){
				temp_page_flash_data_prepare(page_ptr, true);
				flash_upload_flag &= ~TEMP_PAGE_FLAG;
			}
			
			rc = nrf_fstorage_write(&fstorage, start_address, page_ptr, block_size,NULL);//ssl_block_size[page_number], NULL);
			APP_ERROR_CHECK(rc);
			wait_for_flash_ready(&fstorage);
			free(page_ptr);
		}
		
		NRF_LOG_INFO("Write Done. %d",page_number);
}


/**@brief Function for searching the block that the data is written to
 * @details 
 * @param[in] page_number, the flash page to write data into.
 */
// page number 1,2,3,4 ....
static uint32_t get_block_start_address(uint8_t page_number){
	char flag[4]={0};
	ret_code_t rc;
	int index=0;
	
	page_number -= 1;
	uint32_t temp_start_address = 0;
	
		rc = nrf_fstorage_read(&fstorage, page_start_address[page_number], flag, 4);
		APP_ERROR_CHECK(rc);
		if(flag[0]== (char)flash_guard){
			temp_start_address = page_start_address[page_number];
		}
	read_index = index;
	NRF_LOG_INFO("Index%d found from page%d.", read_index,page_number);	
	
	return temp_start_address;
}


/**@brief Function for read config data from 1 block
 * @details 
 * @param[in] start_address, block address.
 */
static void config_page_read_row(uint32_t start_address){
	void * void_ptr;		
	uint16_t y=4;
	
	// Refresh Indexes for page 0
	config_page_flash_data_prepare(void_ptr, false);
			
	// 2.
	y = conf_page_index.cust_name;
	y = byte_flashStr_read((uint8_t *)cust.name, start_address, y, 16);
	NRF_LOG_INFO("Reading user_name \"%s\" from flash.", cust.name);
	// 3.
	y = conf_page_index.cust_phone;
	y = byte_flashStr_read((uint8_t *)cust.phone, start_address, y, 16);
	NRF_LOG_INFO("Reading user_phone \"%s\" from flash.", cust.phone);
	// 4.
	y = conf_page_index.cust_readable_id;
	y = byte_flash32_read(&cust.readable_id, start_address, y, sizeof(cust.readable_id));
	NRF_LOG_INFO("Reading readable_id \"%d\" from flash.", cust.readable_id);
	// 5.
	y = conf_page_index.cust_provisioning_id;
	y = byte_flash32_read(&cust.provisioning_id, start_address, y, sizeof(cust.provisioning_id));
	NRF_LOG_INFO("Reading provisioning_id \"%d\" from flash.", cust.provisioning_id);	
	
	// 6.
	y = conf_page_index.dev_id;
	y = byte_flash32_read(&dev.id, start_address, y, sizeof(dev.id));
	NRF_LOG_INFO("Reading device id \"%d\" from flash.", dev.id);	
	// 7.
	y = conf_page_index.dev_start_code;
	y = byte_flash32_read(&dev.start_code, start_address, y, sizeof(dev.start_code));
	NRF_LOG_INFO("Reading start code \"%d\" from flash.", dev.start_code);	
	// 8.
	y = conf_page_index.dev_provisioning_status;
	y = byte_flash32_read(&dev.provisioning_status, start_address, y, sizeof(dev.provisioning_status));
	NRF_LOG_INFO("Reading provisioning_status \"%d\" from flash.", dev.provisioning_status);	
	// 9.
	y = conf_page_index.dev_server_auth_token;
	y = byte_flashStr_read(dev.server_auth_token, start_address, y, 24);
	NRF_LOG_DEBUG("Reading server_auth_token \"%s\" from flash.", dev.server_auth_token);	
	
	uint32_t dummy;
	// 10.
	y = conf_page_index.battery_device_alert_threshold;
	y = byte_flash32_read(&dummy, start_address, y, sizeof(dummy));
	NRF_LOG_INFO("Reading dummy \"%d\" from flash.", dummy);	
	
	// 11.	
	y = conf_page_index.dev_qa_flag;
	//uint32_t dummy; 	//dfu.cmd	
	y = byte_flash32_read(&dummy, start_address, y, sizeof(dummy));
	NRF_LOG_INFO("Reading dummy \"%d\" from flash.Index:%d", dummy,y);	
	
	// 12.
	y = conf_page_index.payg_is_unlocked;
	y = byte_flash32_read(&payg.is_unlocked, start_address, y, sizeof(payg.is_unlocked));
	NRF_LOG_INFO("Reading payg.is_unlocked \"%d\" from flash.Index:%d", payg.is_unlocked,y);	
	// 13.
	y = conf_page_index.dev_secret;
	y = byte_flashStr_read(dev.secret, start_address, y, 16);
	static char hex_string[50];
	for(uint8_t i=0; i<16; i++){
		sprintf(hex_string+(2*i),"%02X",dev.secret[i]);
	}
	//NRF_LOG_INFO("Received dsc %s \n",hex_string);
	NRF_LOG_DEBUG("Reading secret \"%s\" from flash.", hex_string);
	// 14.
	y = conf_page_index.dev_productive_output_set_limit;
	y = byte_flash32_read(&dev.productive_output_set_limit, start_address, y, sizeof(dev.productive_output_set_limit));
	NRF_LOG_INFO("Reading dev.productive_output_set_limit \"%d\" from flash.Index:%d", dev.productive_output_set_limit,y);
	// 15.
	y = conf_page_index.temp_threshold_temperature;
	y = byte_flash32_read(&temp.threshold_temperature, start_address, y, sizeof(temp.threshold_temperature));
	NRF_LOG_INFO("Reading temp.threshold_temperature \"%d\" from flash.Index:%d", temp.threshold_temperature,y);
	// 16.
	y = conf_page_index.proximity_lock;
	y = byte_flash32_read(&dev.proximity_lock, start_address, y, sizeof(dev.proximity_lock));
	NRF_LOG_INFO("Reading dev.proximity_lock \"%d\" from flash.Index:%d", dev.proximity_lock,y);
	
	NRF_LOG_INFO("Configuration Load Done.");	
}


/**@brief Function for read payg data from 1 block
 * @details 
 * @param[in] start_address, block address.
 * @param[in] is_timeseries, true - the data is history not current data.
 */
static void payg_page_read_row(uint32_t start_address, bool is_timeseries){
	void * void_ptr;		
	uint16_t y=4;
	
	// Refresh Indexes for page 0
	payg_page_flash_data_prepare(void_ptr, false);
	
	
	// 2. payg.credit_remaining
	y = payg_page_index.payg_payG_credit_remaining;
	y = byte_flash32_read(&timeseries.tpayg.payG_credit_remaining, start_address, y, sizeof(payg.payG_credit_remaining));
	NRF_LOG_INFO("Reading payG_credit_remaining \"%d\" from flash.", timeseries.tpayg.payG_credit_remaining);	
	
	// 3. payg.mode
	y = payg_page_index.payg_mode;
	y = byte_flash32_read(&timeseries.tpayg.mode, start_address, y, sizeof(payg.mode));
	NRF_LOG_INFO("Reading mode \"%d\" from flash.", timeseries.tpayg.mode);	
	// 4. payg.last_payg_credit
	y = payg_page_index.payg_last_payg_credit;
	y = byte_flash32_read(&timeseries.tpayg.last_payg_credit, start_address, y, sizeof(payg.last_payg_credit));
	NRF_LOG_INFO("Reading last_payg_credit \"%lu\" from flash.", timeseries.tpayg.last_payg_credit);	
	// 5. payg.last_payG_calc
	y = payg_page_index.payg_last_payG_calc;
	y = byte_flash32_read(&timeseries.tpayg.last_payG_calc, start_address, y, sizeof(payg.last_payG_calc));
	NRF_LOG_INFO("Reading last_payG_calc \"%lu\" from flash.", timeseries.tpayg.last_payG_calc);	
	// 6. payg.last_payG_update
	y = payg_page_index.payg_last_payG_update;
	y = byte_flash32_read(&timeseries.tpayg.last_payG_update, start_address, y, sizeof(payg.last_payG_update));
	NRF_LOG_INFO("Reading last_payG_update \"%lu\" from flash.", timeseries.tpayg.last_payG_update);	
	// 7. payg.token
	char tempstr[20];
	y = payg_page_index.payg_token;
	y = byte_flashStr_read((uint8_t *)tempstr, start_address, y, 20);
	strncpy(timeseries.tpayg.token,tempstr,20);
	NRF_LOG_INFO("Reading token \"%s\" from flash.", timeseries.tpayg.token);	
	// 8. payg.timeseries_index
	y = payg_page_index.timeseries_index;
	y = byte_flash32_read(&timeseries.tpayg.timeseries_index, start_address, y, sizeof(payg.timeseries_index));
	NRF_LOG_INFO("Reading timeseries_index \"%lu\" from flash.", timeseries.tpayg.timeseries_index);	
	
	if(is_timeseries == false){		// Data is current not history
		//payg.payG_credit_remaining = timeseries.tpayg.payG_credit_remaining;
		payg.mode = timeseries.tpayg.mode;
		payg.last_payg_credit = timeseries.tpayg.last_payg_credit;
		payg.last_payG_calc = timeseries.tpayg.last_payG_calc;
		payg.last_payG_update = timeseries.tpayg.last_payG_update;
		strncpy(payg.token,timeseries.tpayg.token,20);
	}
	NRF_LOG_INFO("PAYG Load Done.");	
}


/**@brief Function for reading temperature data from 1 block
 * @details 
 * @param[in] start_address, block address.
 * @param[in] is_timeseries, true - the data is history not current data.
 */
static void temp_page_read_row(uint32_t start_address, bool is_timeseries){
	void * void_ptr;		
	uint16_t y=4;
	
	// Refresh Indexes for page 0
	temp_page_flash_data_prepare(void_ptr, false);
	// 2. max_temperature
	y = temp_page_index.temp_max_temperature;
	y = byte_flash32_read(&timeseries.ttemp.max_temperature, start_address, y, sizeof(temp.max_temperature));
	NRF_LOG_INFO("Reading temp.max_temperature \"%d\" from flash.Index:%d", timeseries.ttemp.max_temperature,y);
	// 3. current_temperature
	y = temp_page_index.temp_current_temperature;
	y = byte_flash32_read(&timeseries.ttemp.current_temperature, start_address, y, sizeof(temp.current_temperature));
	NRF_LOG_INFO("Reading current_temperature \"%lu\" from flash.", timeseries.ttemp.current_temperature);	
	// 4. temp.timeseries_index
	y = temp_page_index.timeseries_index;
	y = byte_flash32_read(&timeseries.ttemp.timeseries_index, start_address, y, sizeof(temp.timeseries_index));
	NRF_LOG_INFO("Reading timeseries_index \"%lu\" from flash.", timeseries.ttemp.timeseries_index);	
	
	
	if(is_timeseries == false){		// Data is current not history
		temp.max_temperature = timeseries.ttemp.max_temperature;
	}
	NRF_LOG_INFO("TEMP Load Done.");	
}


/**@brief Function for read time data from 1 block
 * @details 
 * @param[in] start_address, block address.
 */
static void system_time_page_read_row(uint32_t start_address){
	void * void_ptr;		
	uint16_t y=4;
	
	// Refresh Indexes for page 4
	system_time_page_flash_data_prepare(void_ptr, false);
	// 2. payg.payG_credit_remaining
	y = system_time_page_index.payg_payG_credit_remaining;
	y = byte_flash32_read(&payg.payG_credit_remaining, start_address, y, sizeof(payg.payG_credit_remaining));
	NRF_LOG_INFO("Reading payG_credit_remaining \"%lu\" from flash. Index:%d", payg.payG_credit_remaining,y);	
	
	// 3. system_uptime
	y = system_time_page_index.system_uptime;
	y = byte_flash32_read(&system_uptime, start_address, y, sizeof(system_uptime));
	NRF_LOG_INFO("Reading system_uptime \"%lu\" from flash. Index:%d", system_uptime,y);	
	// 4. proximity_lock_counter
	y = system_time_page_index.proximity_lock_counter;
	y = byte_flash32_read(&dev.proximity_lock_counter, start_address, y, sizeof(dev.proximity_lock_counter));
	NRF_LOG_INFO("Reading dev.proximity_lock_counter \"%lu\" from flash. Index:%d", dev.proximity_lock_counter,y);	
	// 5. unix_time
	//y = system_time_page_index.unix_time;
	//y = byte_flash32_read(&unix_time, start_address, y, sizeof(unix_time));
	//NRF_LOG_INFO("Reading unix_time \"%lu\" from flash. Index:%d", unix_time,y);	
	
	
	NRF_LOG_INFO("System Time Load Done.");	
}


/**@brief Function for erasing a single page from flash
 * @details 
 * @param[in] page_number, page to be erased.
 */
static void ssl_low_erase_page(uint8_t page_number){
	ret_code_t rc;
	
	rc = nrf_fstorage_erase(&fstorage, page_start_address[page_number-1], 1, NULL);
	APP_ERROR_CHECK(rc);
	wait_for_flash_ready(&fstorage);
	NRF_LOG_INFO("Erase Done %d.",page_number);	
}


/**@brief Function for reading boot value from flash
 * @details .
 */
static void ssl_low_fstorage_load(){

	uint32_t conf_page_start_address = get_block_start_address(CONFIG_PAGE);
	
	// If not provisioned clear the flash
	if(conf_page_start_address==0){
		// Erase user flash
		uint8_t page_count;
		for(page_count=1;page_count<=LAST_PAGE;page_count++){
			ssl_low_erase_page(page_count);
		}
		
		flash_upload_flag |= CONFIG_PAGE_FLAG;			// UPDATE Configuration PAGE1
		flash_upload_flag |= SYSTEM_TIME_PAGE_FLAG;	// UPDATE Time PAGE2	
		flash_upload_flag |= NEXUS_PAGE_FLAG;				// UPDATE Nexus Keycode PAGE3	
		flash_upload_flag |= PAYG_PAGE_FLAG;				// PAYG Time 4
		flash_upload_flag |= TEMP_PAGE_FLAG;				// TEMP Time 9
		return;
	}
	
	config_page_read_row(conf_page_start_address);
	uint32_t system_time_page_start_address = get_block_start_address(SYSTEM_TIME_PAGE);
	if(system_time_page_start_address != 0)
		system_time_page_read_row(system_time_page_start_address);
	
	uint32_t payg_page_start_address = get_block_start_address(PAYG_PAGE);
	if(payg_page_start_address != 0)
		payg_page_read_row(payg_page_start_address,false);	
	uint32_t temp_page_start_address = get_block_start_address(TEMP_PAGE);
	if(temp_page_start_address != 0)
		temp_page_read_row(temp_page_start_address,false);	
}

uint32_t nxp_flash_get_index(uint8_t block_id){
	
	uint16_t y =0;
	uint8_t flag_length = 4;
	switch(block_id){
			case 0:
				y=flag_length;
				break;
		  case 1:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+flag_length;
				break;
			#if NEXUS_CHANNEL_LINK_SECURITY_ENABLED
			case 2:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+flag_length;
				break;
			case 3:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+flag_length;
				break;
			case flag_length:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH+flag_length;
				break;
			case 5:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+flag_length;
				break;
			case 6:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+flag_length;
				break;
			case 7:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+NX_COMMON_NV_BLOCK_6_LENGTH+flag_length;
				break;
			case 8:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+NX_COMMON_NV_BLOCK_6_LENGTH+NX_COMMON_NV_BLOCK_7_LENGTH+flag_length;
				break;
			case 9:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+NX_COMMON_NV_BLOCK_6_LENGTH+NX_COMMON_NV_BLOCK_7_LENGTH\
					+NX_COMMON_NV_BLOCK_8_LENGTH+flag_length;
				break;
			case 10:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+NX_COMMON_NV_BLOCK_6_LENGTH+NX_COMMON_NV_BLOCK_7_LENGTH\
					+NX_COMMON_NV_BLOCK_8_LENGTH+NX_COMMON_NV_BLOCK_9_LENGTH+flag_length;
				break;
			case 11:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+NX_COMMON_NV_BLOCK_6_LENGTH+NX_COMMON_NV_BLOCK_7_LENGTH\
					+NX_COMMON_NV_BLOCK_8_LENGTH+NX_COMMON_NV_BLOCK_9_LENGTH+NX_COMMON_NV_BLOCK_10_LENGTH+flag_length;
				break;
			case 12:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+NX_COMMON_NV_BLOCK_6_LENGTH+NX_COMMON_NV_BLOCK_7_LENGTH\
					+NX_COMMON_NV_BLOCK_8_LENGTH+NX_COMMON_NV_BLOCK_9_LENGTH+NX_COMMON_NV_BLOCK_10_LENGTH+NX_COMMON_NV_BLOCK_11_LENGTH+flag_length;
				break;
			case 13:
				y+=NX_COMMON_NV_BLOCK_0_LENGTH+NX_COMMON_NV_BLOCK_1_LENGTH+NX_COMMON_NV_BLOCK_2_LENGTH+NX_COMMON_NV_BLOCK_3_LENGTH\
					+NX_COMMON_NV_BLOCK_4_LENGTH+NX_COMMON_NV_BLOCK_5_LENGTH+NX_COMMON_NV_BLOCK_6_LENGTH+NX_COMMON_NV_BLOCK_7_LENGTH\
					+NX_COMMON_NV_BLOCK_8_LENGTH+NX_COMMON_NV_BLOCK_9_LENGTH+NX_COMMON_NV_BLOCK_10_LENGTH+NX_COMMON_NV_BLOCK_11_LENGTH\
					+NX_COMMON_NV_BLOCK_12_LENGTH+flag_length;
				break;
			#endif
			default:
				return error_code;
		}
		return y;
}

uint32_t nxp_flash_get_start_address(uint8_t page_number){
		uint32_t start_address = 0;
		start_address=0;
		page_number--;
		uint32_t flag=0;
		
			ret_code_t rc = nrf_fstorage_read(&fstorage, page_start_address[page_number], &flag, 4);
			if(flag ==flash_guard){
				start_address = page_start_address[page_number];//+(ssl_block_size[page_number]*index);
			}
		
		return start_address;
}
	
bool nxp_common_nv_write(const struct nx_common_nv_block_meta block_meta, void* write_buffer)
{
	 uint8_t nx_nv_block_0_ram[block_meta.length];
	 nxp_common_nv_read(block_meta, &nx_nv_block_0_ram);
	 bool identical = (bool) (memcmp(nx_nv_block_0_ram, write_buffer,block_meta.length)==0);
	 // Do not write if identical
	 if (identical)
	 {
			NRF_LOG_INFO("No need of writing nv_id %d \"%s\" to flash.", block_meta.block_id,(char *)write_buffer);
		 	return true;
	 }
	 
	 uint8_t nexus_block_backup[nexus_block_size];
	 
	 uint32_t start_address= nxp_flash_get_start_address(4);
	 if(start_address == 0){
			NRF_LOG_INFO("Start Address fetch failed.");
			nexus_block_backup[0] = flash_guard; nexus_block_backup[1] = 0; nexus_block_backup[2] = 0; nexus_block_backup[3] = 0;
	 }
	 else{
			byte_flashStr_read(nexus_block_backup, start_address, 0, nexus_block_size);
	 }
	 
	 uint32_t fetched_index;
	 if((fetched_index = nxp_flash_get_index(block_meta.block_id)) == error_code){
			NRF_LOG_INFO("Index fetch failed.");
			return false;
	 }
	 uint16_t y= (uint16_t) fetched_index;
	 memcpy(nexus_block_backup+y, write_buffer, block_meta.length);
	 //char flag[4];
	 ret_code_t rc;
	 const uint8_t page_number = NEXUS_PAGE-1;
	 start_address = 0;

	 if(start_address == 0){										// page is full erase and start from the start of page
			rc = nrf_fstorage_erase(&fstorage, page_start_address[page_number], 1, NULL);
			APP_ERROR_CHECK(rc);
			wait_for_flash_ready(&fstorage);
			NRF_LOG_INFO("Erase Done.");
			start_address = page_start_address[page_number];
	}
		
	 rc = nrf_fstorage_write(&fstorage, start_address, nexus_block_backup, nexus_block_size, NULL);
	 APP_ERROR_CHECK(rc);
	 wait_for_flash_ready(&fstorage);
			 
		////return nonvol_update_block(block_meta.block_id,write_buffer);
		NRF_LOG_INFO("Writing nv_id %d to flash.", block_meta.block_id);	
		return true;
	 
}

bool nxp_common_nv_read(const struct nx_common_nv_block_meta block_meta, void* read_buffer)
{
		uint32_t start_address= nxp_flash_get_start_address(NEXUS_PAGE);
	 if(start_address==0){
			// not provisioned run demo
			return false;
		}
			
		
		char tempstr[60];
		uint32_t fetched_index = 0;
		if((fetched_index = nxp_flash_get_index(block_meta.block_id)) == error_code){
			return false;
		}
		uint16_t y= (uint16_t) fetched_index;
		
		y = byte_flashStr_read((uint8_t*) tempstr, start_address, y, block_meta.length);
		memcpy(read_buffer, tempstr, block_meta.length);
		return true;
}


/**@brief Function for searching timeseries data in the flash
 * @details 
 * @param[in] page_number, page to search.
 */
static uint32_t get_start_address_by_marker(uint8_t page_number){	
	page_number -= 1;
	uint32_t temp_start_address = 0;
	
	if(page_number == PAYG_PAGE-1){
		;
	}
	else if(page_number == TEMP_PAGE-1){
		;
	}
	else{
		return 0;
	}
	
	return temp_start_address;
}


/**@brief Function for Marking the block that is already read
 * @details 
 */
static void timeseries_save_marker(void){
	
	uint32_t temp_start_address = marker_address;
	uint32_t temp_read_marker = 0;
	uint16_t read_marker_index = marker_index;
	ret_code_t rc;
	flash_upload_flag &= ~MARKER_FLAG;
	if(temp_start_address==0){
		return;
	}
	
	byte_flash32_read(&temp_read_marker, temp_start_address, read_marker_index, sizeof(uint32_t));
	if(temp_read_marker==0xFFFFFFFF && dfu.auth){
		//1. write read mark (4 of 4 Bytes)
		char *ptr;
		ptr = (char*)malloc(sizeof(uint32_t));
		byte_flash32_add((uint8_t *) ptr, 0,read_mark);
		rc = nrf_fstorage_write(&fstorage, temp_start_address+read_marker_index, ptr, 4, NULL);
		APP_ERROR_CHECK(rc);
		wait_for_flash_ready(&fstorage);
		free(ptr);	
	}
}

bool update_data_by_marker(uint8_t page_number){
	uint32_t page_start_address = get_start_address_by_marker(page_number);
	
	if(page_start_address != 0){
		if(page_number == PAYG_PAGE){
			payg_page_read_row(page_start_address,true);
		}
		else if(page_number == TEMP_PAGE){
			temp_page_read_row(page_start_address,true);
		}
		else{
			return false;
		}
		return true;
	}
	else{
		return false;
	}
}
