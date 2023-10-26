/**
 * Copyright (c) 2020 - 2021, Simusolar Limited
 *
 * All rights reserved.
 *
 * SSL Master Definitions File
 * This file contains globsl variables used by other source files
 */
 
#ifdef DEFINE_GLOBALS_HERE
#undef  GLOBAL
#define GLOBAL
#else
#undef  GLOBAL
#define GLOBAL extern
#endif
#include <stdint.h>
GLOBAL uint32_t system_uptime;																	///< Timestamp of uptime in minutes								(4/4 Bytes)
GLOBAL uint16_t flash_upload_flag;																///< Flag to indicate if there is data to be updated in flash 0-no, 1-yes
GLOBAL const uint32_t flash_guard;
		
#include "ble_dfu.h"
GLOBAL	ble_dfu_buttonless_t             m_dfu;  
#ifndef Customer_Provision
#define Customer_Provision
  struct customer
	{
    const uint32_t resource_version;
	  char name[17];
    char phone[17];  
    uint32_t readable_id;
		uint8_t provisioning_id;
		uint8_t * server_auth_token;
		uint8_t * provisioning_status;			// Provisioning Status "pst"			
		const uint16_t rtr;
	};	
#endif
GLOBAL struct customer cust;  
	
#ifndef Device_Provision
#define Device_Provision
  struct device
	{
    const uint32_t resource_version;
	  uint32_t id; 														// Device ID "did" 							
		const char payg_units_accepted[2]; 			// PayG Units accepted "pul"
    uint32_t start_code;										// Payg Token starting code "psc"		
		uint8_t secret[16];											// Nexus Payg Secret
		const char payg_unit[2]; 								// PayG Unit "pu"			
		const uint8_t*  timeseries_data_format;	// BLE TimeSeries Data Format "tdf"			
		uint8_t provisioning_status;			// Provisioning Status "pst"			
		uint8_t server_auth_token[21];  	// Server Auth Token "sat"			
		uint8_t openPAYGO_data_format;		// OpenPAYGO Metrics Data format "df"			
		// firmware specific variables
		uint32_t productive_output_set_limit;	//Productive Output Set Limit "opmax"
		uint32_t proximity_lock;	// Duration (in seconds) in which the light can be powered on without syncing with the authorized PayG gateway
		uint32_t proximity_lock_counter;	// Duration (in seconds) in which the light can be powered on without syncing with the authorized PayG gateway
		uint32_t time_calibration_factor[2];
		const uint16_t rtr;
	};	
#endif
GLOBAL struct device dev;  
			
#ifndef PAYG
#define PAYG
  struct payg_resource
	{
    const uint32_t resource_version;					// AirLink PAYG resource version "rv"				
		uint32_t payG_credit_remaining;						// Nexus credit 				
		uint16_t is_unlocked;											// Nexus uint16_t to pad to half-word alignment on 4-byte architecture// 0 = not unlocked, 1 = unlocked
		uint8_t mode;															// Mode "mo"				
		char token[20];														// PayG Token "tkn"				
		uint32_t last_payg_credit;								// Last Added PayG Credit "lcr"				
		uint32_t last_payG_calc;									// Timestamp at which PayG remaining was calculated "ts"				
		uint32_t last_payG_update;								// Timestamp of last PAYG Update to device "lts"				
		uint32_t local_time;											// Current Local Time "lt"				
		uint32_t timeseries_index;								// current row of data of timeseries "ti"
		const uint16_t rtr;
	};	
#endif
GLOBAL struct payg_resource payg;  
	
#ifndef DFU
#define DFU
  struct DFU_resource
	{
    const uint32_t resource_version;		// resource version "rv"				
		const uint16_t fw_version;					// FW Version "fv"		
		const uint32_t file_size;						// File size accepted? "fs"		
		const uint16_t hw_version;					// HW version? "hv"		
		const char default_sat[21];								// Update command "cmd"	
		bool auth;
		const uint16_t rtr;
	};	
#endif
GLOBAL struct DFU_resource dfu;  

#ifndef temperature
#define temperature
  struct temperature_resource
	{
    const uint32_t resource_version;		// resource version "rv"				
		uint32_t current_temperature;				// Current Temperature "temp"
		uint32_t max_temperature;						// Max Temperature "tmax"
		uint32_t threshold_temperature;			// Temperature upper limit threshold "tlim"
		uint32_t timeseries_index;								// current row of data of timeseries "ti"
		const uint16_t rtr;
	};	
#endif
GLOBAL struct temperature_resource temp;  	
	
#ifndef Timeseries_Container
#define Timeseries_Container
  struct timeseries_resource
	{
		struct payg_resource tpayg;
		struct device tdev;
		uint32_t system_uptime;
		struct temperature_resource ttemp;
		
		const uint16_t rtr;
	};	
#endif
GLOBAL struct timeseries_resource timeseries;  

#ifndef PUE_RESOURCE
#define PUE_RESOURCE
  struct pue_resource
	{
		const uint32_t resource_version;			// resource version "rv"				
		const uint8_t data_format;						// Time Series Data Format "df"				
		const uint8_t equipment_type;					// Productive Equipment Type "pue"				
		const uint8_t battery_device;					// BatteryDevice "bat"				
		uint8_t device_fault;									// Device Fault "ft"				
		//char device_fault_data[17];					// Device Fault Data "ftd"				
		uint32_t device_fault_data;						// Device Fault Data "ftd"				
		uint32_t seconds_since_data_measured;	// seconds since data measured "ss"				
		uint32_t * primary_metric; 						// Productive Output Primary Metric "op"				
		uint32_t secondary_metric; 						// Productive Output Secondary Metric "os"
		uint32_t reset_source;
		uint32_t timeseries_index;						// current row of data of timeseries "ti"
		const uint16_t rtr;
	};	
#endif
GLOBAL struct pue_resource pue;  

#ifndef advertisement
#define advertisement
  struct advertisement_resource
	{
    const uint32_t resource_version;		// resource version "rv"				
		
		const uint16_t rtr;
	};	
#endif
GLOBAL struct advertisement_resource advert;  
	
#ifdef DEFINE_GLOBALS_HERE
	struct payg_resource payg = {
    .resource_version = 1,						// AirLink PAYG resource version "rv"				
		.payG_credit_remaining = 0,	// Device PayG Credit Remaining "re"
		
		.is_unlocked = 0,
		.mode = 0, 												// Mode "mo"				
		.token = "*Start Token#",					// PayG Token "tkn"				
		.last_payg_credit = 7200,					// Last Added PayG Credit "lcr"				
		.last_payG_calc = 1630585953,			// Timestamp at which PayG remaining was calculated "ts"				
		.last_payG_update = 1630585957,		// Timestamp of last PAYG Update to device "lts"				
		.local_time = 0,									// Current Local Time "lt"				
		.timeseries_index = 0,
		.rtr = 65001
	};
	
	struct DFU_resource dfu = {
    .resource_version = 1,		// AirLink PAYG resource version "rv"				
		.fw_version = 0x0105,					// FW Version "fv"		
		.file_size = 100000,			// File size accepted? "fs"		
		.hw_version = 1,					// HW version? "hv"		
		.default_sat="92WwT5ZYXtKuUtZrJZAs",				// Update command "cmd"		
		.auth = false,
		.rtr = 65004
	};

	struct timeseries_resource timeseries = {
    .rtr = 65005
	};
	
		
	struct temperature_resource temp = {
    .resource_version = 1,
	  .current_temperature = 20,
		.max_temperature = 20,
		.threshold_temperature = 100,
		.timeseries_index = 0,
		.rtr = 65011
	};
	
	struct customer cust = {
    .resource_version = 1,
		.readable_id = 0,
		.provisioning_id = 0,
		.name = "AIRLINK CUSTOMER",
		.phone = "(+255)0000000000",
	  .server_auth_token = &dev.server_auth_token[0],
		.provisioning_status = &dev.provisioning_status,
		.rtr = 65002
  };
	
	struct device dev = {
    .resource_version = 1,
	  .id = 0,
		.secret = {0xDE,0xAD,0xBE,0xEF,0x10,0x20,0x30,0x40,0x04,0x03,0x02,0x01,0xFE,0xEB,0xDA,0xED},
		.payg_units_accepted = "s",
	  .start_code = 0,																			// Payg Token starting code "psc"			
		.payg_unit = "s", 																		// PayG Unit "pu"			
		.timeseries_data_format = &pue.data_format,						// BLE TimeSeries Data Format "tdf"			
		.provisioning_status = PROVISIONING_STATUS_UNSERIALIZED,// Provisioning Status "pst"			
		.server_auth_token = {0},  														// Server Auth Token "sat"			
		.openPAYGO_data_format = 0,														// OpenPAYGO Metrics Data format "df"			
		.time_calibration_factor = {60,1647988718},
		.rtr = 65003
	};
	
	uint32_t system_uptime		 = 5;																	///< Timestamp of uptime in minutes								(4/4 Bytes)
	uint32_t unix_time = 0;																			///< Timestamp of uptime in minutes								(4/4 Bytes)
	uint16_t flash_upload_flag = 0;																		///< Flag to indicate if there is data to be updated in flash 0-no, 1-yes
	const uint32_t flash_guard = 15;
#endif

