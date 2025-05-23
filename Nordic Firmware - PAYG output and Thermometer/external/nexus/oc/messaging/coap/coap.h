/*
// Copyright (c) 2016 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/
/*
 *
 * Copyright (c) 2013, Institute for Pervasive Computing, ETH Zurich
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the Contiki operating system.
 *
 * Modifications (c) 2020 Angaza, Inc.
 */

#ifndef COAP_H
#define COAP_H

#include "conf.h"
#include "constants.h"
#include <stddef.h> /* for size_t */
#include <stdint.h>

#include "oc_buffer.h"
#include "oc_config.h"
#include "port/oc_connectivity.h"

#include "port/oc_log.h"
#include "port/oc_random.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MAX
    #define MAX(n, m) (((n) < (m)) ? (m) : (n))
#endif

#ifndef MIN
    #define MIN(n, m) (((n) < (m)) ? (n) : (m))
#endif

#ifndef ABS
    #define ABS(n) (((n) < 0) ? -(n) : (n))
#endif

/* bitmap for set options */
enum
{
    OPTION_MAP_SIZE = sizeof(uint8_t) * 8
};

#define SET_OPTION(packet, opt)                                                \
    ((packet)->options[opt / OPTION_MAP_SIZE] |= 1 << (opt % OPTION_MAP_SIZE))
#define IS_OPTION(packet, opt)                                                 \
    ((packet)->options[opt / OPTION_MAP_SIZE] & (1 << (opt % OPTION_MAP_SIZE)))

/* enum value for coap transport type  */
typedef enum
{
    COAP_TRANSPORT_UDP,
    COAP_TRANSPORT_TCP
} coap_transport_type_t;

/* parsed message struct */
typedef struct
{
    uint8_t* buffer; /* pointer to CoAP header / incoming packet buffer / memory
                        to serialize packet */
    coap_transport_type_t transport_type;
    uint8_t version;
    coap_message_type_t type;
    uint8_t code;
    uint16_t mid;

    uint8_t token_len;
    uint8_t token[COAP_TOKEN_LEN];

    uint8_t options[10]; /* bitmap to check if option is set */

    uint16_t content_format; /* parse options once and store; allows setting
                                options in random order  */
    uint32_t max_age;
    uint8_t etag_len;
    uint8_t etag[COAP_ETAG_LEN];
    size_t proxy_uri_len;
    const char* proxy_uri;
    size_t proxy_scheme_len;
    const char* proxy_scheme;
    size_t uri_host_len;
    const char* uri_host;
    size_t location_path_len;
    const char* location_path;
    uint16_t uri_port;
    size_t location_query_len;
    const char* location_query;
    size_t uri_path_len;
    const char* uri_path;
    int32_t observe;
    uint16_t accept;
    uint8_t if_match_len;
    uint8_t if_match[COAP_ETAG_LEN];
    uint32_t block2_num;
    uint8_t block2_more;
    uint16_t block2_size;
    uint32_t block2_offset;
    uint32_t block1_num;
    uint8_t block1_more;
    uint16_t block1_size;
    uint32_t block1_offset;
    uint32_t size2;
    uint32_t size1;
    size_t uri_query_len;
    const char* uri_query;
    uint8_t if_none_match;

    uint32_t payload_len;
    uint8_t* payload;
} coap_packet_t;

/* option format serialization */
#define COAP_SERIALIZE_INT_OPTION(number, field, text)                         \
    if (IS_OPTION(coap_pkt, number))                                           \
    {                                                                          \
        option_length += coap_serialize_int_option(                            \
            number, current_number, option, coap_pkt->field);                  \
        if (option)                                                            \
        {                                                                      \
            option = option_array + option_length;                             \
        }                                                                      \
        current_number = number;                                               \
    }
#define COAP_SERIALIZE_BYTE_OPTION(number, field, text)                        \
    if (IS_OPTION(coap_pkt, number))                                           \
    {                                                                          \
        option_length += coap_serialize_array_option(number,                   \
                                                     current_number,           \
                                                     option,                   \
                                                     coap_pkt->field,          \
                                                     coap_pkt->field##_len,    \
                                                     '\0');                    \
        if (option)                                                            \
        {                                                                      \
            option = option_array + option_length;                             \
        }                                                                      \
        current_number = number;                                               \
    }
#define COAP_SERIALIZE_STRING_OPTION(number, field, splitter, text)            \
    if (IS_OPTION(coap_pkt, number))                                           \
    {                                                                          \
        option_length +=                                                       \
            coap_serialize_array_option(number,                                \
                                        current_number,                        \
                                        option,                                \
                                        (uint8_t*) coap_pkt->field,            \
                                        coap_pkt->field##_len,                 \
                                        splitter);                             \
        if (option)                                                            \
        {                                                                      \
            option = option_array + option_length;                             \
        }                                                                      \
        current_number = number;                                               \
    }
#define COAP_SERIALIZE_BLOCK_OPTION(number, field, text)                       \
    if (IS_OPTION(coap_pkt, number))                                           \
    {                                                                          \
        uint32_t block = coap_pkt->field##_num << 4;                           \
        if (coap_pkt->field##_more)                                            \
        {                                                                      \
            block |= 0x8;                                                      \
        }                                                                      \
        block |= 0xF & coap_log_2(coap_pkt->field##_size / 16);                \
        option_length +=                                                       \
            coap_serialize_int_option(number, current_number, option, block);  \
        if (option)                                                            \
        {                                                                      \
            option = option_array + option_length;                             \
        }                                                                      \
        current_number = number;                                               \
    }

/* to store error code and human-readable payload */
extern coap_status_t coap_status_code;
extern char* coap_error_message;

void coap_init_connection(void);
uint16_t coap_get_mid(void);

void coap_udp_init_message(void* packet,
                           coap_message_type_t type,
                           uint8_t code,
                           uint16_t mid);
size_t coap_serialize_message(void* packet, uint8_t* buffer);
coap_status_t
coap_udp_parse_message(void* request, uint8_t* data, uint16_t data_len);

/*---------------------------------------------------------------------------*/

int coap_set_status_code(void* packet, unsigned int code);

int coap_set_token(void* packet, const uint8_t* token, size_t token_len);

int coap_get_header_content_format(void* packet, unsigned int* format);
int coap_set_header_content_format(void* packet, unsigned int format);

size_t coap_get_header_uri_path(
    void* packet,
    const char** path); /* in-place string might not be 0-terminated. */
size_t
coap_set_header_uri_path(void* packet, const char* path, size_t path_len);

size_t coap_get_header_uri_query(
    void* packet,
    const char** query); /* in-place string might not be 0-terminated. */
size_t coap_set_header_uri_query(void* packet, const char* query);

/*
int coap_get_header_observe(void* packet, uint32_t* observe);
int coap_set_header_observe(void* packet, uint32_t observe);
*/

int coap_get_payload(void* packet, const uint8_t** payload);
int coap_set_payload(void* packet, const void* payload, size_t length);

#ifdef __cplusplus
}
#endif

#endif /* COAP_H */
