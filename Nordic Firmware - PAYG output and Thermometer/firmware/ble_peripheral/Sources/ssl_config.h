/**
 * Copyright (c) 2020 - 2021, Simusolar Limited
 *
 * All rights reserved.
 *
 * SSL Master Configuration File
 * This file contains configurations used by other source files
 */
#include "ssl_global_variables.h" 
#ifndef SSL_CONFIG_H
#define SSL_CONFIG_H

//==========================================================
// SSL Specific configurations
//==========================================================
//BL653
#ifdef BOARD_PCA10100																																// Send Huada MCU to wake it up

#ifndef MULTIPURPOSE_KEY																														// Key for everything
#define MULTIPURPOSE_KEY 	20		//20 in production, 12 in development																												
#endif

#ifndef LED1																																				// Debug LED1
#define LED1 31//16
#endif

#ifndef LED2																																				// Debug LED2
#define LED2 32			//32 in production, 15 in development
#endif

#ifndef PAYG_LED																																				// Debug LED2 from DVK-652-sa
#define PAYG_LED 13
#endif

//DFU_KEY is used if bootloader code
#ifndef DFU_KEY																																			// Key to force BL652 to go into DFU mode
#define DFU_KEY 35
#endif

//BL652
#elif BOARD_PCA10040																																// Send Huada MCU to wake it up

#ifndef MULTIPURPOSE_KEY																														// Key for everything
#define MULTIPURPOSE_KEY 11																													// Same as button2 on DVK-652-sa
#endif


#ifndef LED1																																				// Debug LED1 from DVK-652-sa
#define LED1 30//17
#endif

#ifndef LED2																																				// Debug LED2 from DVK-652-sa
#define LED2 31//19
#endif


#ifndef PAYG_LED																																				// Debug LED2 from DVK-652-sa
#define PAYG_LED 19
#endif


/* DFU_KEY is used if bootloader code
#ifndef DFU_KEY																																			// Key to force BL652 to go into DFU mode
#define DFU_KEY 13
#endif
*/
#endif
// BL652/3 END	


//==========================================================
// Definitions required by provisioning
//==========================================================

#define PROVISIONING_STATUS_UNSERIALIZED 	1
#define PROVISIONING_STATUS_UNPROVISIONED 2
#define PROVISIONING_STATUS_DISABLED 			3
#define PROVISIONING_STATUS_RECALL				4
#define PROVISIONING_STATUS_STOLEN 				5
#define PROVISIONING_STATUS_CASH 					6
#define PROVISIONING_STATUS_LOAN 					7
#define PROVISIONING_STATUS_SERIALIZED 	101
#define PROVISIONING_STATUS_PROVISIONED 102
#define PROVISIONING_STATUS_PAYG_STATE_CHANGED 103

//==========================================================
// Definitions required by ble_resources_definitions.c
//==========================================================

//#ifdef BOARD_PCA10100
//#define DEVICE_NAME                     "NRF52833"                        					/**< Name of device. Will be included in the advertising data. */
//#elseif BOARD_PCA10040
//#define DEVICE_NAME                     "NRF52832"                        					/**< Name of device. Will be included in the advertising data. */
//#else

#define DEVICE_NAME                     "AIRLINK"                        						/**< Name of device. Will be included in the advertising data. */
//#endif

#define MANUFACTURER_NAME               "SIMUSOLAR"                       					/**< Manufacturer. Will be passed to Device Information Service. */
#define APP_ADV_INTERVAL                300                                         /**< The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms). */
#define APP_ADV_DURATION                0		   //3000                              	/**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */

#define BUTTON_DETECTION_DELAY          APP_TIMER_TICKS(50)                     		/**< Delay from a GPIOTE event until a button is reported as pushed (in number of timer ticks). */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(200, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */

#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAMS_UPDATE_COUNT    3                                           /**< Number of attempts before giving up the connection parameter negotiation. */

#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                           /**< Man In The Middle protection not required. */
#define SEC_PARAM_LESC                  0                                           /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                        /**< No I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define DEAD_BEEF                       0xDEADBEEF                                  /**< Value used as error code on stack dump, can be used to identify stack location on stack unwind. */

// ble_resources_definitions.c
//==========================================================
// Definitions required by ble_device_discovery_service.c
//==========================================================
/**@brief   Macro for defining a ble_device_discovery_service instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_DEVICE_DISCOVERY_SERVICE_BLE_OBSERVER_PRIO 2                                                      \
// ble_device_discovery_service service:97a0d761-d5ec-11eb-b8bc-0242ac130003
// resources_enumerator characteristic: 97a0d762-d5ec-11eb-b8bc-0242ac130003

// The bytes are stored in little-endian format, meaning the
// Least Significant Byte is stored first
// (reversed from the order they're displayed as)

// Base UUID: 97a0a4f0-d5ec-11eb-b8bc-0242ac130003
#define BLE_UUID_SSL_BASE_UUID  {0x03, 0x00, 0x13, 0xAC, 0x42, 0x02, 0xBC, 0xB8, 0xEB, 0x11, 0xEC, 0xD5, 0x00, 0x00, 0xA0, 0x97}
#define BLE_UUID_DEVICE_DISCOVERY_SERVICE_BASE_UUID	BLE_UUID_SSL_BASE_UUID
// Service & characteristics UUIDs
#define BLE_UUID_DEVICE_DISCOVERY_SERVICE_UUID  0xD761
#define BLE_UUID_RESOURCES_ENUMERATOR_CHAR_UUID   0xD762
//==========================================================
// Definitions required by ble_pue_use_service.c
//==========================================================
/**@brief   Macro for defining a ble_pue_use_service instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_PUE_USE_SERVICE_BLE_OBSERVER_PRIO 2                                                      \
// pue_use service:                     97a0a4f1-d5ec-11eb-b8bc-0242ac130003
// PayG characteristic:   					 		97a0a4f2-d5ec-11eb-b8bc-0242ac130003

// The bytes are stored in little-endian format, meaning the
// Least Significant Byte is stored first
// (reversed from the order they're displayed as)

// Base UUID: 97a0a4f0-d5ec-11eb-b8bc-0242ac130003
//#define BLE_UUID_SSL_BASE_UUID  {0x03, 0x00, 0x13, 0xAC, 0x42, 0x02, 0xBC, 0xB8, 0xEB, 0x11, 0xEC, 0xD5, 0x00, 0x00, 0xA0, 0x97}
#define BLE_UUID_PUE_USE_SERVICE_BASE_UUID	BLE_UUID_SSL_BASE_UUID
// Service & characteristics UUIDs
#define BLE_UUID_PUE_USE_SERVICE_UUID  0xA4F1
#define BLE_UUID_PAYG_CHAR_UUID   0xA4F2
#define BLE_UUID_TIMESERIES_CHAR_UUID   0xA4F3
#define BLE_UUID_TEMPERATURE_CHAR_UUID   0xA4F9

// ble_pue_use_update_service.c
//==========================================================
// Definitions required by ble_device_config_service.c
//==========================================================
#define BLE_DEVICE_CONFIG_SERVICE_BLE_OBSERVER_PRIO 2
// Provision service:                     20ca8d71-d5f9-11eb-b8bc-0242ac130003
// Provision characteristic:   					 20ca8d72-d5f9-11eb-b8bc-0242ac130003

// The bytes are stored in little-endian format, meaning the
// Least Significant Byte is stored first
// (reversed from the order they're displayed as)

// Base UUID: 20ca8d70-d5f9-11eb-b8bc-0242ac130003
//{0x03, 0x00, 0x13, 0xAC, 0x42, 0x02, 0xBC, 0xB8, 0xEB, 0x11, 0xF9, 0xD5, 0x70, 0x8D, 0xCA, 0x20}

#define BLE_UUID_DEVICE_CONFIG_SERVICE_BASE_UUID  BLE_UUID_SSL_BASE_UUID
// Service & characteristics UUIDs
#define BLE_UUID_DEVICE_CONFIG_SERVICE_UUID  0x8D71
#define BLE_UUID_DEVICE_PROVISION_CHAR_UUID   0x8D72
#define BLE_UUID_CLIENT_PROVISION_CHAR_UUID   0x8D73
#define BLE_UUID_DFU_CHAR_UUID   0x8D74

// ble_device_config_service.c
//==========================================================
// Definitions required by gfx_definitions.c
//==========================================================
#define GRAY            0xC618
#define RED             0xF800
#define BLUE            0x001F
#define WHITE           0xFFFF
#define BLACK						0x0000
#define LIGHT_GRAY			0xBDF7
#define DARK_GRAY				0x7BEF
#define YELLOW					0xFFE0
#define ORANGE					0xFBE0
#define BROWN						0x79E0
#define GREEN						0x07E8//0x07E0
#define CYAN 						0x07FF
#define PINK						0xF81F
#define SSL							0x600C

#define LINE_STEP       10

#define CIRCLE_RADIUS   10
#define CIRCLE_STEP     ((2 * CIRCLE_RADIUS) + 1)

#define BORDER          0

// gfx_definitions.c
//==========================================================
// Definitions required by ble_pwr_mgmt_definitions.c
//==========================================================

////// No config variables

// ble_pwr_mgmt_definitions.c
//==========================================================
// Definitions required by ble_timers_definitions.c
//==========================================================
#include "nrf_drv_rtc.h"
#include "ble_gpio_config.h" 

#define rtc_tick_time_min 				 1																			/**< Time between every wake up. */
#define rtc_timeseries_flash_save_time_min 	 (rtc_tick_time_min*30)				/**<30 Time between flash saves, in terms of rtc_tick_time_min i.e 3 means 3*1=3min. */
#define rtc_huada_update_query_min 1																			/**< Time between Asking updates from huada, should be less than rtc_tick_time_min. */
#define rtc_system_time_flash_save_time_min 	 (rtc_tick_time_min*10)			/**< Time between flash saves, in terms of rtc_tick_time_min i.e 3 means 3*1=3min. */

#define COMPARE_COUNTERTIME  (rtc_tick_time_min*60)                                 /**<60 Get Compare event COMPARE_TIME seconds after the counter starts from 0. */

#ifdef BSP_LED_0
    #define TICK_EVENT_OUTPUT     BSP_LED_0                                 				/**< Pin number for indicating tick event. */
#elif defined(LED1)
		#define TICK_EVENT_OUTPUT     LED1                                 							/**< Pin number for indicating tick event. */
#endif
//#ifndef TICK_EVENT_OUTPUT
//    #error "Please indicate output pin"
//#endif
#ifdef BSP_LED_1
    #define COMPARE_EVENT_OUTPUT   BSP_LED_1                               					/**< Pin number for indicating compare event. */
#elif defined(LED2)
		#define COMPARE_EVENT_OUTPUT   LED2                               							/**< Pin number for indicating compare event. */  
#endif
//#ifndef COMPARE_EVENT_OUTPUT
//    #error "Please indicate output pin"
//#endif
static const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2); 													/**< Declaring an instance of nrf_drv_rtc for RTC2. */

// ble_timers_definitions.c
//==========================================================
// Definitions required by ble_gpio_config.c
//==========================================================

#define power_sw_time_min							0
#define power_sw_time_max							99999//19999
//#define brightness_control_time				20000
#define multipurpose_key_timeout			100000

// ble_gpio_config.c
//==========================================================
// Definitions required by ble_comm_definitions.c
//==========================================================

#if defined (UART_PRESENT)
#include "nrf_uart.h"
#endif
#if defined (UARTE_PRESENT)
#include "nrf_uarte.h"
#endif

#define UART_TX_BUF_SIZE                256                                         /**< UART TX buffer size. */
#define UART_RX_BUF_SIZE                256                                         /**< UART RX buffer size. */


// ble_comm_definitions.c
//==========================================================
// Definitions required by ble_flash_fstorage_definitions.c
//==========================================================

// <o> CYCLIC_SAVE_METHOD_ENABLED
 
// <0=> Do not use cyclic method, i.e erase before every write (offer high reliabilty, low Flash Endurance) 
// <1=> Use cyclic method, i.e erase when page is full (offer low reliabilty, high Flash Endurance) 

#ifndef CYCLIC_SAVE_METHOD_ENABLED
#define CYCLIC_SAVE_METHOD_ENABLED 1
#endif

// huada_definitions.c
//==========================================================
// Definitions required by ble_flash_fstorage_definitions.c
//==========================================================

#define FL_TURN_OFF 0X01
#define FL_BRIGHTNESS_SETTING 0X02
#define FL_READ_BRIGHTNESS 0X03
#define FL_TURN_ON 0X04
#define FL_LED_FLASH 0X05
#define FL_DISPLAY 0X06
#define FL_READ_CHARGE_PERCENTAGE 0X07
#define FL_READ_CHARGING_STATE 0X08
#define FL_READ_CHARGE 0X09
#define FL_READ_CHARGE_TIME 0X0B
#define FL_READ_CURRENT 0X0C
#define FL_READ_VOLTAGE 0X0D

#define FL_ERROR 0XFF
#define FL_CHARGING_UPDATE 0X01
#define FL_NOT_CHARGING_UPDATE 0X02
#define FL_BATTERY_PROTECTION_ENTER 0X03

//==========================================================
// Definitions required by NEXUS KEYCODE
//==========================================================

#define CONFIG_NEXUS_COMMON_ENABLED 1
#define CONFIG_NEXUS_KEYCODE_ENABLED 1
#define CONFIG_NEXUS_KEYCODE_USE_FULL_KEYCODE_PROTOCOL 1
#define CONFIG_NEXUS_KEYCODE_PROTOCOL_RATE_LIMIT_BUCKET_MAX 128
#define CONFIG_NEXUS_KEYCODE_PROTOCOL_RATE_LIMIT_BUCKET_INITIAL_COUNT 80
#define CONFIG_NEXUS_KEYCODE_PROTOCOL_RATE_LIMIT_REFILL_SECONDS_PER_ATTEMPT 720
#define CONFIG_NEXUS_KEYCODE_ENABLE_FACTORY_QC_CODES 1
#define CONFIG_NEXUS_KEYCODE_PRO_FACTORY_QC_SHORT_LIFETIME_MAX 5
#define CONFIG_NEXUS_KEYCODE_PRO_FACTORY_QC_LONG_LIFETIME_MAX 5
#define CONFIG_NEXUS_KEYCODE_PROTOCOL_ENTRY_TIMEOUT_SECONDS 16

#ifdef DEBUG
	#define CONFIG_NEXUS_KEYCODE_RATE_LIMITING_ENABLED 0
#else
	#define CONFIG_NEXUS_KEYCODE_RATE_LIMITING_ENABLED 1
#endif



#endif //SSL_CONFIG_H
