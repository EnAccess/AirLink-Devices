/** \file feedback.c
 * \brief Implementation of Nexus Keycode and Channel feedback (for UI).
 * \author Angaza
 * \copyright 2020 Angaza, Inc.
 * \license This file is released under the MIT license
 *
 * The above copyright notice and license shall be included in all copies
 * or substantial portions of the Software.
 */

//#include "nxp_channel.h"
#include "nxp_keycode.h"
#include <stdio.h>
#include "nrf_log.h"
#include "payg_state.h"
//#include "gfx_definitions.h"

bool nxp_keycode_feedback_start(enum nxp_keycode_feedback_type feedback_type)
{
    uint8_t feedback_id = 0;
		pue.device_fault_data &= 0XFF00FFFF;
		switch (feedback_type)
    {
        case NXP_KEYCODE_FEEDBACK_TYPE_MESSAGE_INVALID:
            if (nx_keycode_is_rate_limited())
            {
                NRF_LOG_INFO(
                    "\tNexus Keycode is rate-limited, please wait %d seconds "
                    "before entering another keycode.\n",
                    CONFIG_NEXUS_KEYCODE_PROTOCOL_RATE_LIMIT_REFILL_SECONDS_PER_ATTEMPT);
								//tft_write_payg_feedback(1, 10);
								feedback_id = 1;
								pue.device_fault_data |= (feedback_id<<16);
            }
            else
            {
                NRF_LOG_INFO("\tKeycode is invalid.\n");
								//tft_write_payg_feedback(2, 10);
								feedback_id = 2;
								pue.device_fault_data |= (feedback_id<<16);
            }
            break;
        case NXP_KEYCODE_FEEDBACK_TYPE_MESSAGE_APPLIED:
						payg_state_update_nv();			// Force to save payg status in storage
            NRF_LOG_INFO("\tKeycode is valid.\n");
						pue.device_fault_data |= (0xff<<16);
            break;
        case NXP_KEYCODE_FEEDBACK_TYPE_MESSAGE_VALID:
            NRF_LOG_INFO("\tKeycode is valid; but, is either a duplicate or had no "
                   "effect.\n");
						//tft_write_payg_feedback(3, 10);
						feedback_id = 3;
						pue.device_fault_data |= (feedback_id<<16);
            break;
        case NXP_KEYCODE_FEEDBACK_TYPE_KEY_REJECTED:
#if CONFIG_NEXUS_KEYCODE_USE_FULL_KEYCODE_PROTOCOL
            NRF_LOG_INFO("\tInvalid key entry. Full keycodes must be entered without "
                   "spaces and in the form of *(0-9)#.\n");
						//tft_write_payg_feedback(4, 10);
						feedback_id = 4;
						pue.device_fault_data |= (feedback_id<<16);
#elif CONFIG_NEXUS_KEYCODE_USE_SMALL_KEYCODE_PROTOCOL
            NRF_LOG_INFO("\tInvalid key entry. Small keycodes must be entered "
                   "without spaces and in the form of 1-5.\n");
						//tft_write_payg_feedback(5, 10);
						feedback_id = 5;
						pue.device_fault_data |= (feedback_id<<16);
#else
    #error "Error: Keycode protocol configuration missing..."
#endif
            break;
        case NXP_KEYCODE_FEEDBACK_TYPE_DISPLAY_SERIAL_ID:;
            NRF_LOG_INFO("\tSerial ID is %u.\n", nxp_keycode_get_user_facing_id());
            break;
        case NXP_KEYCODE_FEEDBACK_TYPE_KEY_ACCEPTED:
        case NXP_KEYCODE_FEEDBACK_TYPE_NONE:
        case NXP_KEYCODE_FEEDBACK_TYPE_RESERVED:
        default:
            return false;
    }
		NRF_LOG_DEBUG("feedback_id:%d",feedback_id);
		return true;
}
/*
void nxp_channel_notify_event(enum nxp_channel_event_type event)
{

    switch (event)
    {
        case NXP_CHANNEL_EVENT_LINK_ESTABLISHED_AS_ACCESSORY:
            printf("\tCHANNEL EVENT: Link established as *accessory* device "
                   "(%u total links)\n",
                   nx_channel_link_count());
            break;

        case NXP_CHANNEL_EVENT_LINK_ESTABLISHED_AS_CONTROLLER:
            printf("\tCHANNEL EVENT: Link established as *controller* device "
                   "(%u total links)\n",
                   nx_channel_link_count());
            break;

        case NXP_CHANNEL_EVENT_LINK_DELETED:
            printf(
                "\tCHANNEL EVENT: A link has been deleted (%u links remain)\n",
                nx_channel_link_count());
            break;

        case NXP_CHANNEL_EVENT_LINK_HANDSHAKE_STARTED:
            printf("\tCHANNEL EVENT: Beginning link handshake\n");
            break;

        case NXP_CHANNEL_EVENT_LINK_HANDSHAKE_TIMED_OUT:
            printf("\tCHANNEL EVENT: Link handshake timed out, no link "
                   "created.\n");
            break;
            break;
    }
}
*/
