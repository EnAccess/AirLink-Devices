/** \file identity.c
 * \brief A mock implementation of one way to store device identity.
 * \author Angaza
 * \copyright 2020 Angaza, Inc.
 * \license This file is released under the MIT license
 *
 * The above copyright notice and license shall be included in all copies
 * or substantial portions of the Software.
 */

#include "identity.h"
//#include "nonvol.h"
#include "nx_keycode.h"
//#include "nxp_channel.h"
#include "nxp_common.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssl_config.h"

static struct identity_struct _this;

uint32_t nxp_keycode_get_user_facing_id(void)
{
    return _this.serial_id;
}

struct nx_id nxp_channel_get_nexus_id(void)
{
    struct nx_id my_id;
    my_id.authority_id = 0;
    my_id.device_id = nxp_keycode_get_user_facing_id();
    return my_id;
}

struct nx_common_check_key nxp_keycode_get_secret_key(void)
{
    return _this.secret_key;
}

struct nx_common_check_key nxp_channel_symmetric_origin_key(void)
{
    // In a real system, different keys are used for keycode validation and
    // Nexus Channel 'origin' authentication, but we use the same key in this
    // demo for simplicity.
    return _this.secret_key;
}

void identity_init(void)
{
    //_this.serial_id = 0;
    //memset(_this.secret_key.bytes, 0, sizeof(_this.secret_key.bytes));
		_this.serial_id = dev.id;
		//uint8_t secret[] = {0xde,0xad,0xbe,0xef,0x10,0x20,0x30,0x40,0x04,0x03,0x02,0x01,0xfe,0xeb,0xda,0xed};
		strcpy((char*)_this.secret_key.bytes,(char*)dev.secret);
		//_this.secret_key.bytes = {0xde,0xad,0xbe,0xef,0x10,0x20,0x30,0x40,0x04,0x03,0x02,0x01,0xfe,0xeb,0xda,0xed};
		/*
    // attempt to read from NV
    bool valid_nv =
        prod_nv_read_identity(sizeof(struct identity_struct), (void*) &_this);

    // If we have a valid identity in NV, don't attempt to write a new one.
    if (valid_nv)
    {
        return;
    }

    printf("Please enter an integer serial ID.\n");
    if (scanf("%u", (uint32_t*) &_this.serial_id) != 1)
    {
        printf("Unable to parse the serial ID.\n");
        exit(1);
    }

    printf("Please enter the 16-byte hexidecimal secret key. For example, "
           "\"deadbeef1020304004030201feebdaed\".\n");
    for (size_t count = 0; count < sizeof(_this.secret_key.bytes) /
                                       sizeof(*_this.secret_key.bytes);
         count++)
    {
        if (scanf("%2hhx", &_this.secret_key.bytes[count]) != 1)
        {
            printf("Unable to parse the secret key.\n");
            exit(1);
        }
    }
    // If read failed, write our results from above to NV
    prod_nv_write_identity(sizeof(struct identity_struct), (void*) &_this);
		*/
}
