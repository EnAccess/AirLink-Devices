#ifndef BLE_FLASH_FSTORAGE_DEFINITIONS_H_
	#define BLE_FLASH_FSTORAGE_DEFINITIONS_H_
	#include "nrf_fstorage.h"
	#ifdef SOFTDEVICE_PRESENT
		#include "nrf_sdh.h"
		#include "nrf_sdh_ble.h"
		#include "nrf_fstorage_sd.h"
	#else
		#include "nrf_drv_clock.h"
		#include "nrf_fstorage_nvmc.h"
	#endif

	#include "nrf_log.h"
	#include "nrf_log_ctrl.h"
	#include "nrf_log_default_backends.h"
  #include "ssl_config.h"
	static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);
	
	/* Dummy data to write to flash. */
	//static uint32_t m_data          = 0xBADC0FFE;
	//static char     m_hello_world[] = "hello world";

	uint32_t nrf5_flash_end_addr_get(void);
	void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage);
	void print_flash_info(nrf_fstorage_t * p_fstorage);
	void flash_init(void);
	void ssl_flash_update(void);
	void ssl_flash_load(void);
	static void ssl_low_fstorage_write(uint8_t page_number);
	static void ssl_low_fstorage_load(void);
	bool update_data_by_marker(uint8_t page_number);
	static void timeseries_save_marker(void);
	
	extern const uint16_t CONFIG_PAGE_FLAG;
	extern const uint16_t SYSTEM_TIME_PAGE_FLAG;
	extern const uint16_t NEXUS_PAGE_FLAG;
	extern const uint16_t PAYG_PAGE_FLAG;
	extern const uint16_t TEMP_PAGE_FLAG;
	extern const uint16_t TIMESERIES_FLAG;
	extern const uint16_t MARKER_FLAG;
	
	extern const uint8_t CONFIG_PAGE;
	extern const uint8_t SYSTEM_TIME_PAGE;
	extern const uint8_t NEXUS_PAGE;
	extern const uint8_t PAYG_PAGE;
	extern const uint8_t TEMP_PAGE;
	#endif 
