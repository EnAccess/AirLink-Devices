/*This file contains defitions for ble power management library*/
#include "ble_pwr_mgmt_definitions.h"
#include "nrf_sdh.h"
#include "bsp_btn_ble.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_log.h"
#include "nrf_delay.h"
#include "nxp_common.h"
#include "ble_resource_definitions.h"
#include "nrf_power.h"
#include "ble_flash_fstorage_definitions.h"
//#include "ble_comm_definitions.h"
#include "app_timer.h"
// FINDME
static bool system_on_state_flag= true;																	// Flag to indicate if system is power on or not; false-power off, true-power on
static bool proximity_lock_state_flag= false;

/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
void idle_state_handle(void)
{
		if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


void power_management_init(void)
{
    uint32_t err_code = nrf_pwr_mgmt_init();
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for the Power manager.
 */
void log_init(void)
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}


/**@brief Function for putting the chip into sleep mode.
 *
 * @note This function will not return.
 */
void sleep_mode_enter(void)
{
    uint32_t err_code = bsp_indication_set(BSP_INDICATE_IDLE);
    APP_ERROR_CHECK(err_code);

		nrf_gpio_pin_clear(LED1);
		nrf_gpio_pin_clear(LED2);
    //Disable SoftDevice. It is required to be able to write to GPREGRET2 register (SoftDevice API blocks it).
    //GPREGRET2 register holds the information about skipping CRC check on next boot.
    err_code = nrf_sdh_disable_request();
    APP_ERROR_CHECK(err_code);
}

/* Power management handlers*/
/**@brief Handler for shutdown preparation.
 *
 * @details During shutdown procedures, this function will be called at a 1 second interval
 *          untill the function returns true. When the function returns true, it means that the
 *          app is ready to reset to DFU mode.
 *
 * @param[in]   event   Power manager event.
 *
 * @retval  True if shutdown is allowed by this power manager handler, otherwise false.
 */
bool app_shutdown_handler(nrf_pwr_mgmt_evt_t event)
{
    switch (event)
    {
        case NRF_PWR_MGMT_EVT_PREPARE_DFU:
            NRF_LOG_INFO("Power management wants to reset to DFU mode.");
            // YOUR_JOB: Get ready to reset into DFU mode
            //
            // If you aren't finished with any ongoing tasks, return "false" to
            // signal to the system that reset is impossible at this stage.
            //
            // Here is an example using a variable to delay resetting the device.
            //
            // if (!m_ready_for_reset)
            // {
            //      return false;
            // }
            // else
            //{
            //
            //    // Device ready to enter
            //    uint32_t err_code;
            //    err_code = sd_softdevice_disable();
            //    APP_ERROR_CHECK(err_code);
            //    err_code = app_timer_stop_all();
            //    APP_ERROR_CHECK(err_code);
            //}
            break;
				default:
            // YOUR_JOB: Implement any of the other events available from the power management module:
            //      -NRF_PWR_MGMT_EVT_PREPARE_SYSOFF
            //      -NRF_PWR_MGMT_EVT_PREPARE_WAKEUP
            //      -NRF_PWR_MGMT_EVT_PREPARE_RESET
            return true;
    }

    NRF_LOG_INFO("Power management allowed to reset to DFU mode.");
    return true;
}


/**@brief This function will return the state of proximity_lock.
 *
 */
bool get_proximity_status(void){
	return proximity_lock_state_flag;
}

/**@brief This function will Change the state of proximity_lock.
 *
 */

void change_proximity_status(bool proximity_status){
	proximity_lock_state_flag = proximity_status;
}

/**@brief This function will return the state of system.
 *
 */
bool get_system_status(void){
	return system_on_state_flag;
}

/**@brief This function will Change the state of system.
 * This fuction behaves like the power switch, when true is passed the output will be enabled
 */

void change_system_status(bool system_status){
	system_on_state_flag = system_status;
	if(system_status){
		if((nxp_common_payg_state_get_current()!=NXP_COMMON_PAYG_STATE_DISABLED) && !get_proximity_status()){
			NRF_LOG_INFO("Output Enabled");
			nrf_gpio_pin_set(PAYG_LED);
		}
		else{
			NRF_LOG_INFO("Output PAYG Disabled");
			nrf_gpio_pin_clear(PAYG_LED);
		}
	}
	else if(!system_status) {
		NRF_LOG_INFO("Output Disabled");
		nrf_gpio_pin_clear(PAYG_LED);
	}
}

/**@brief This function handles system state changes.
 *
 */

void system_state_handler(void){

	static uint8_t previous_payg_state = 0x77;
	static uint16_t temperature_read_count = 0;
		
	// Handle Temperature Changes
	temp.current_temperature= get_temperature();
	if((temp.current_temperature>temp.max_temperature) || temperature_read_count++>=1440){// 24h
		temp.max_temperature = temp.current_temperature;
		temperature_read_count = 0;
	}
	
	// Handle Proximity state changes
	// From unlocked to locked
	static uint8_t previous_proximity_lock_status = 77;
	if(previous_proximity_lock_status != get_proximity_status()){
		if(previous_proximity_lock_status != 77){
			change_system_status(get_system_status());
			NRF_LOG_INFO("Proximity Lock system off");
		}
		previous_proximity_lock_status = get_proximity_status();
	}
	
	// Handle payg state changes
	// From Enable/unlocked to disabled
	if((nxp_common_payg_state_get_current()==NXP_COMMON_PAYG_STATE_DISABLED) && \
		 (previous_payg_state)){
			change_system_status(get_system_status());
			 previous_payg_state = 0;
	}
	// From Disabled to Enabled/unlocked	 
	if((nxp_common_payg_state_get_current()!=NXP_COMMON_PAYG_STATE_DISABLED) && \
		 (!previous_payg_state || previous_payg_state==0x77)){
			change_system_status(get_system_status());
			 previous_payg_state = 1;
	}
}

void provision_state_handler(uint8_t new_provision_state){
	static bool start = false;
	
	if(dev.id<10000){						// Not serialized, need serial number
		new_provision_state = PROVISIONING_STATUS_UNSERIALIZED;
	}
	else if((strcmp((char*)dev.server_auth_token,"")==0) && (new_provision_state != PROVISIONING_STATUS_PROVISIONED)){	//Not SAT, needs provision
		new_provision_state = PROVISIONING_STATUS_UNPROVISIONED;
	}
	else if(PROVISIONING_STATUS_PAYG_STATE_CHANGED && (new_provision_state != PROVISIONING_STATUS_PROVISIONED)){
		new_provision_state = PROVISIONING_STATUS_PAYG_STATE_CHANGED; 
	}
	else
		;	// Provision State above are device controlled, the rest will go here
	NRF_LOG_DEBUG("new_provision_state: %d",new_provision_state);	
	if(new_provision_state == dev.provisioning_status && start){	
		return;
	}
	else{
		start = true;
		switch(new_provision_state){
			case PROVISIONING_STATUS_UNSERIALIZED:
				dev.provisioning_status = PROVISIONING_STATUS_UNSERIALIZED;
				dev.id=1000+(NRF_FICR->DEVICEID[0]%9000);
				break;
			case PROVISIONING_STATUS_UNPROVISIONED:
				dev.provisioning_status = PROVISIONING_STATUS_UNPROVISIONED;
				break;
			
			case PROVISIONING_STATUS_SERIALIZED:
				if(strcmp((char*)dev.server_auth_token,"")==0){
					dev.provisioning_status = PROVISIONING_STATUS_UNPROVISIONED;
					break;
				}
				//if(dev.provisioning_status<=PROVISIONING_STATUS_UNSERIALIZED)
				//	dev.provisioning_status = PROVISIONING_STATUS_UNPROVISIONED;
				//break;
			case PROVISIONING_STATUS_PROVISIONED:
				if(dev.provisioning_status>PROVISIONING_STATUS_UNPROVISIONED){
					break;
				}
			case PROVISIONING_STATUS_PAYG_STATE_CHANGED:	
				if(nxp_common_payg_state_get_current()==NXP_COMMON_PAYG_STATE_UNLOCKED){
					dev.provisioning_status = PROVISIONING_STATUS_CASH;
				}
				else{
					dev.provisioning_status = PROVISIONING_STATUS_LOAN;
				}
				break;
				
			case PROVISIONING_STATUS_DISABLED:			// todo: Incomplete logic
				dev.provisioning_status = PROVISIONING_STATUS_DISABLED;
				break;
			case PROVISIONING_STATUS_RECALL:				// todo: Incomplete logic
				dev.provisioning_status = PROVISIONING_STATUS_RECALL;
				break;
			case PROVISIONING_STATUS_STOLEN:				// todo: Incomplete logic
				dev.provisioning_status = PROVISIONING_STATUS_STOLEN;
				break;
			/*
			case PROVISIONING_STATUS_CASH:				// todo: Incomplete logic
				dev.provisioning_status = PROVISIONING_STATUS_CASH;
				break;
			
			case PROVISIONING_STATUS_LOAN:				// todo: Incomplete logic
				dev.provisioning_status = PROVISIONING_STATUS_LOAN;
				break;
			*/
			default:
				break;
		}
	}
	advertising_update_data();
}

void reset_source_handler(void){
	//uint32_t reset_reason; 
	uint32_t reset_reason = NRF_POWER->RESETREAS;
	NRF_POWER->RESETREAS = 0xffffffff;
	//reset_reason = nrf_power_resetreas_get();
	//nrf_power_resetreas_clear(NRF_POWER_RESETREAS_OFF_MASK);
	pue.device_fault_data &= 0XFFFFFF00;
	if(reset_reason&0x0001){
		NRF_LOG_INFO("Reset reason = Reset Pin\r\n");
		pue.device_fault_data |= 0x01;
	}
	if(reset_reason&0x00000002){
		NRF_LOG_INFO("Reset reason = Watchdog\r\n");
		pue.device_fault_data |= 0x02;
	}
	if(reset_reason&0x00000004){
		NRF_LOG_INFO("Reset reason = Soft Reset\r\n");
		pue.device_fault_data |= 0x03;
	}
	if(reset_reason&0x00000008){
		NRF_LOG_INFO("Reset reason = CPU LockUP\r\n");
		pue.device_fault_data |= 0x04;
	}
	if(reset_reason&0x00010000){
		NRF_LOG_INFO("Reset reason = Wakeup from system off sleep by GPIO\r\n");
		pue.device_fault_data |= 0x05;
	}
	if(reset_reason&0x00020000){
		NRF_LOG_INFO("Reset reason = Wakeup from system off sleep by LPCOMP\r\n");
		pue.device_fault_data |= 0x06;
	}
	if(reset_reason&0x00040000){
		NRF_LOG_INFO("Reset reason = Wakeup from system off sleep by Debug Interface\r\n");
		pue.device_fault_data |= 0x07;
	}
	if(reset_reason&0x00080000){
		NRF_LOG_INFO("Reset reason = Wakeup from system off sleep by NFC\r\n");
		pue.device_fault_data |= 0x08;
	}
	if(reset_reason&0x00100000){
		NRF_LOG_INFO("Reset reason = Wakeup from system off sleep by VBUS\r\n");
		pue.device_fault_data |= 0x09;
	}
	pue.reset_source = reset_reason;
	pue.reset_source = 0;
	NRF_LOG_INFO("Reset reason = 0x%08x\r\n", pue.reset_source);
}


//lint -esym(528, m_app_shutdown_handler)
/**@brief Register application shutdown handler with priority 0.
 */
NRF_PWR_MGMT_HANDLER_REGISTER(app_shutdown_handler, 0);


