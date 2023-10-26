#include "ble_timers_definitions.h"

/**@brief Function for the Timer initialization.
 *
 * @details Initializes the timer module. This creates and starts application timers.
 */
#include "app_timer.h" 
#include "nrf_log.h"
#include "ble_resource_definitions.h"
//extern uint32_t system_uptime;																// Timestamp of uptime in minutes								(4/4 Bytes)
//extern uint16_t payG_credit_remaining;												// Device PayG Credit Remaining???? (2/4 Bytes)
//extern uint8_t flash_upload_flag;															// Flag to indicate if there is data to be updated in flash 0-no, 1-page1, 2-page2
#include "ble_flash_fstorage_definitions.h"
#include "ble_pwr_mgmt_definitions.h"
//#include "gfx_definitions.h"

void timers_init(void)
{

    // Initialize timer module.
    uint32_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
		
		// Create timers.

    /* YOUR_JOB: Create any timers to be used by the application.
                 Below is an example of how to create a timer.
                 For every new timer needed, increase the value of the macro APP_TIMER_MAX_TIMERS by
                 one.
       uint32_t err_code;
       err_code = app_timer_create(&m_app_timer_id, APP_TIMER_MODE_REPEATED, timer_timeout_handler);
       APP_ERROR_CHECK(err_code); */
}




/**@brief Function for starting timers.
 */
void application_timers_start(void)
{
    /* YOUR_JOB: Start your timers. below is an example of how to start a timer.
       uint32_t err_code;
       err_code = app_timer_start(m_app_timer_id, TIMER_INTERVAL, NULL);
       APP_ERROR_CHECK(err_code); */
}


/** @brief: Function for handling the RTC2 interrupts.
 * Triggered on TICK and COMPARE2 match.
 */
void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
	  
    if (int_type == NRF_DRV_RTC_INT_COMPARE2)
    {
      //NRF_LOG_INFO("%lu",NRF_RTC2->COUNTER);
			//printf("%d",NRF_RTC2->COUNTER);
			nrf_drv_rtc_counter_clear(&rtc);
			nrf_drv_rtc_int_enable(&rtc, NRF_RTC_INT_COMPARE2_MASK);     
			#ifdef COMPARE_EVENT_OUTPUT
				nrf_gpio_pin_toggle(COMPARE_EVENT_OUTPUT);
			#endif
			//advertising_start(false);
			//uint16_t system_uptime_hrs_old = system_uptime/60;																			// Convert old uptime to hours
			system_uptime += rtc_tick_time_min;																											// Increment uptime with reference to tick time
			//unix_time += rtc_tick_time_min*60;
			
			static uint16_t timeseries_flash_wait_to_upload = 0;
			if(++timeseries_flash_wait_to_upload>=rtc_timeseries_flash_save_time_min){
				flash_upload_flag |= TIMESERIES_FLAG;
				timeseries_flash_wait_to_upload = 0;
			}
			static uint16_t system_time_flash_wait_to_upload = 0;
			if(++system_time_flash_wait_to_upload>=rtc_system_time_flash_save_time_min){
				flash_upload_flag |= SYSTEM_TIME_PAGE_FLAG;
				system_time_flash_wait_to_upload = 0;
			}
			
			if(dev.proximity_lock && !get_proximity_status()){
				dev.proximity_lock_counter += rtc_tick_time_min*60;
			}
			
			if(dev.proximity_lock && (dev.proximity_lock_counter>=dev.proximity_lock)){
				change_proximity_status(true);
			}
			//NRF_LOG_INFO("dev.proximity_lock:%d, dev.proximity_lock_counter%d, get_proximity_status:%d",\
			//dev.proximity_lock,dev.proximity_lock_counter,get_proximity_status());
    }
    else if (int_type == NRF_DRV_RTC_INT_TICK)
    {
        #ifdef TICK_EVENT_OUTPUT
					nrf_gpio_pin_toggle(TICK_EVENT_OUTPUT);
				#endif
    }
		NRF_LOG_INFO("system_uptime: %lu, proximity_lock_counter: %lu",system_uptime,dev.proximity_lock_counter);
}


/** @brief Function initialization and configuration of RTC driver instance.
 */
void rtc_config(void)
{
    uint32_t err_code;

    //Initialize RTC instance
    nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
    config.prescaler = 4095;
    err_code = nrf_drv_rtc_init(&rtc, &config, rtc_handler);
    APP_ERROR_CHECK(err_code);

    //Enable tick event & interrupt
    //nrf_drv_rtc_tick_enable(&rtc,true);

    //Set compare channel to trigger interrupt after COMPARE_COUNTERTIME seconds
    err_code = nrf_drv_rtc_cc_set(&rtc,2,COMPARE_COUNTERTIME * 8,true);
    APP_ERROR_CHECK(err_code);

    //Power on RTC instance
    nrf_drv_rtc_enable(&rtc);
}
