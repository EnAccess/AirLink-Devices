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

#ifndef TRANSACTIONS_H
#define TRANSACTIONS_H

#include "coap.h"
#include "util/oc_etimer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if NEXUS_CHANNEL_USE_OC_OBSERVABILITY_AND_CONFIRMABLE_COAP_APIS
    /*
     * Modulo mask (thus +1) for a random number to get the tick number for the
     * random
     * retransmission time between COAP_RESPONSE_TIMEOUT and
     * COAP_RESPONSE_TIMEOUT*COAP_RESPONSE_RANDOM_FACTOR.
     */
    #define COAP_RESPONSE_TIMEOUT_TICKS                                        \
        (OC_CLOCK_SECOND * COAP_RESPONSE_TIMEOUT)
    #define COAP_RESPONSE_TIMEOUT_BACKOFF_MASK                                 \
        (long) ((OC_CLOCK_SECOND * COAP_RESPONSE_TIMEOUT *                     \
                 ((float) COAP_RESPONSE_RANDOM_FACTOR - 1.0)) +                \
                0.5) +                                                         \
            1
#endif // NEXUS_CHANNEL_USE_OC_OBSERVABILITY_AND_CONFIRMABLE_COAP_APIS

/* container for transactions with message buffer and retransmission info */
typedef struct coap_transaction
{
    struct coap_transaction* next; /* for LIST */
    uint16_t mid;
    struct oc_etimer retrans_timer;
    uint8_t retrans_counter;
#if NEXUS_CHANNEL_LINK_SECURITY_ENABLED
    // used to keep an outbound transaction in case we need to retransmit it
    // due to nonce sync
    struct oc_etimer idle_timeout_timer;
#endif
    oc_message_t* message;
} coap_transaction_t;

void coap_register_as_transaction_handler(void);

int coap_transactions_free_count(void);
coap_transaction_t* coap_new_transaction(uint16_t mid,
                                         const oc_endpoint_t* endpoint);

void coap_send_transaction(coap_transaction_t* t, bool should_cache);
void coap_clear_transaction(coap_transaction_t* t);
coap_transaction_t* coap_get_transaction_by_mid(uint16_t mid);

void coap_check_transactions(void);
void coap_free_all_transactions(void);

// Currently unused, don't compile in
// void coap_free_transactions_by_endpoint(const oc_endpoint_t* endpoint);

#ifdef __cplusplus
}
#endif

#endif /* TRANSACTIONS_H */
