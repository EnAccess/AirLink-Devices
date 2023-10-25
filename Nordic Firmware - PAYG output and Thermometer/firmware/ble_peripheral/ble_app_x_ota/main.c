/**Simusolar Inc 
 * @file
 * @author Lawrence Peter
 * @date 4th June 2021
 * @defgroup Simusolar_FL_main
 * @{
 * @ingroup Simusolar_FL_api
 * @brief Simusolar FL project main file.
 * @section Introduction
 *
 * This file contains the source code for a sample application using the proprietary
 * This project is based on Secure DFU Buttonless Service from nordic SDK
 * Secure DFU Buttonless Service. This is a template application that can be modified
 * to your needs. To extend the functionality of this application, please find
 * locations where the comment "// YOUR_JOB:" is present and read the comments.
 */
 #include <stdio.h>
 #include <stdbool.h>
 #include <stdint.h>
 #include <string.h>
 #include <stdlib.h>
 #include "ble_resource_definitions.h"
 #include "ble_pwr_mgmt_definitions.h"
// #include "gfx_definitions.h"
 #include "nrf_delay.h"
 #include "nrf_log.h"
 #include "ble_gpio_config.h"
 #include "ble_timers_definitions.h"
 //#include "ble_comm_definitions.h"
 #include "ble_flash_fstorage_definitions.h"
 //#include "huada_definitions.h"
 #include "src/internal_keycode_config.h"
 #include "include/nxp_common.h"
 #include "include/nxp_keycode.h"
 //#include "include/nxp_channel.h"
 #include "src/nexus_channel_res_payg_credit.h"
 #include "clock.h"
 #include "payg_state.h"
 #include "identity.h"
 #include "processing.h"
 #include "screen.h"
 #define DEFINE_GLOBALS_HERE
 #include "ssl_config.h"
 
		
/**@brief Function for application main entry.
 */
int main(void)
{
  // Local Varibles declaration
		bool       erase_bonds;
    ret_code_t err_code;
		
	// We need to find reser source early on before the register is overwritten
		reset_source_handler();
    log_init();
		
		// Initialize the async SVCI interface to bootloader before any interrupts are enabled.
    err_code = ble_dfu_buttonless_async_svci_init();
    APP_ERROR_CHECK(err_code);
		
		// Initialize Flash Storage
		flash_init();
		ssl_flash_load();	
		//payg.payG_credit_remaining = 3600;
				
		// Initialize other Modules
		buttons_leds_init(&erase_bonds);
	  ble_stack_init();
    peer_manager_init();
    gap_params_init();
    gatt_init();
		services_init();
		advertising_init();
    gpio_init();
		rtc_config();
		timers_init();
		power_management_init();
		conn_params_init();
		
		// Start execution.
    application_timers_start();
    advertising_start(erase_bonds);
		
		// Nexus Keycode-based Initialization 
		clock_init();
		identity_init();
		payg_state_init();
		nx_common_init(clock_read_monotonic_time_seconds());
		processing_init();
		
		NRF_LOG_INFO("DFU Application started.");
		
		// When there device ID is not assigned this function will assign a temporary ID 	
		provision_state_handler(0);
		// Enter main loop.
		for (;;)
    {    
			// nexus Keycode-based fuctions
			clock_consume_credit();
			screen_display_status();
			processing_execute();
			
			// Handle any system changes (Power,key events, PayG)
			system_state_handler();
			
			// Update advertsing data
			advertising_update_data();
			
			// Update flash (Committed with flash upload flag) 
			ssl_flash_update();
			
			// Prevent Sleep while there are still data to read
			idle_state_handle();
    }
}

/**
 * @}
 */
