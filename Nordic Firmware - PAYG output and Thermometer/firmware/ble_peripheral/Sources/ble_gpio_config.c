#include "ble_gpio_config.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "ble_resource_definitions.h"
#include "ble_pwr_mgmt_definitions.h"
#include "app_timer.h"
#include "nrf_drv_gpiote.h"
#include "nrf_log.h"
//#include "huada_definitions.h"
//#include "gfx_definitions.h"

/**@brief Function for handling events from the button handler module.
 *
 * @param[in] pin_no        The pin that the event applies to.
 * @param[in] button_action The button action (press/release).
 */
static void button_event_handler(uint8_t pin_no, uint8_t button_action)
{    
    switch (pin_no)
    {				
				case MULTIPURPOSE_KEY:
						NRF_LOG_DEBUG("MULTIPURPOSE KEY state %d.", button_action);
						break;

        default:
            APP_ERROR_HANDLER(pin_no);
            break;
    }
}

void gpio_init(void){							// Initialize gpios, some gpios are initilized in their special function files i.e SPI, UART
 /*  Inputs: keys and button
	*  DFU_KEY initilized in bootloader code
	*  NRF_GPIO_PIN_NOPULL   = GPIO_PIN_CNF_PULL_Disabled,                 ///<  Pin pullup resistor disabled
  *  NRF_GPIO_PIN_PULLDOWN = GPIO_PIN_CNF_PULL_Pulldown,                 ///<  Pin pulldown resistor enabled
  *  NRF_GPIO_PIN_PULLUP   = GPIO_PIN_CNF_PULL_Pullup,                   ///<  Pin pullup resistor enabled
	*  nrf_gpio_cfg_sense_input(pin, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);    /// trigger event in low state
	*/
  //#define ADVERTISING_LED                 LED1                         /**< Is on when device is advertising. */
  //#define CONNECTED_LED                   LED2                         /**< Is on when device has connected. */
	
	ret_code_t err_code;
	
	//The array must be static because a pointer to it will be saved in the button handler module.
	static app_button_cfg_t buttons[] =
	{
			{MULTIPURPOSE_KEY, false, NRF_GPIO_PIN_PULLUP, button_event_handler}
	};
	err_code = app_button_init(buttons, ARRAY_SIZE(buttons), BUTTON_DETECTION_DELAY);
	APP_ERROR_CHECK(err_code);
	err_code =app_button_enable();
	APP_ERROR_CHECK(err_code);

	/* Outpus: Leds and others
	 * ST7735_LEDK_PIN initilized in gfx
	 * HUADA_WAKEUP are initialized in huada_definition
	 */
	nrf_gpio_cfg_output(LED1);
	nrf_gpio_cfg_output(LED2);
	nrf_gpio_cfg_output(PAYG_LED);
	uint32_t counter=0;
	while(!nrf_gpio_pin_read(MULTIPURPOSE_KEY)&&counter<15000){
		counter++;
		nrf_delay_ms(1);
	}
	if(counter>10000 && (pue.reset_source&0x04)){
		ble_dfu_buttonless_bootloader_start_finalize();
	}
}


/**@brief Function for initializing buttons and leds.
 *
 * @param[out] p_erase_bonds  Will be true if the clear bonding button was pressed to wake the application up.
 */
void buttons_leds_init(bool * p_erase_bonds)
{
	#if BUTTONS_NUMBER > 0
    uint32_t err_code;
    bsp_event_t startup_event;

    err_code = bsp_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS, bsp_event_handler);
    APP_ERROR_CHECK(err_code);

    err_code = bsp_btn_ble_init(NULL, &startup_event);
    APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
	#else
	  //uint32_t err_code;
    bsp_event_t startup_event;

    //err_code = bsp_init(BSP_INIT_LEDS, bsp_event_handler);
    //APP_ERROR_CHECK(err_code);

    *p_erase_bonds = (startup_event == BSP_EVENT_CLEAR_BONDING_DATA);
	#endif // BUTTONS_NUMBER > 0

}

/**@brief Function for initializing buttons and leds.
 *
 * @details Initializes all LEDs used by the application.
 */
void leds_init()
{
     bsp_board_init(BSP_INIT_LEDS);
}


/**@brief Function for handling events from the BSP module.
 *
 * @param[in]   event   Event generated when button is pressed.
 */
void bsp_event_handler(bsp_event_t event)
{
    uint32_t err_code;

    switch (event)
    {
        case BSP_EVENT_SLEEP:
            sleep_mode_enter();
            break; // BSP_EVENT_SLEEP

        case BSP_EVENT_DISCONNECT:
            err_code = sd_ble_gap_disconnect(m_conn_handle,
                                             BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            if (err_code != NRF_ERROR_INVALID_STATE)
            {
                APP_ERROR_CHECK(err_code);
            }
            break; // BSP_EVENT_DISCONNECT

        case BSP_EVENT_WHITELIST_OFF:
            if (m_conn_handle == BLE_CONN_HANDLE_INVALID)
            {
                err_code = ble_advertising_restart_without_whitelist(&m_advertising);
                if (err_code != NRF_ERROR_INVALID_STATE)
                {
                    APP_ERROR_CHECK(err_code);
                }
            }
            break; // BSP_EVENT_KEY_0

        default:
            break;
    }
}

uint32_t get_temperature(void){
		int32_t temperature_read;
		sd_temp_get(&temperature_read);
		temperature_read *= 0.25;				
		//NRF_LOG_INFO("Temperature: %lu",temperature);
		if(temperature_read<0){
			temperature_read = 0;
		}	
		return (uint32_t)temperature_read;
}
