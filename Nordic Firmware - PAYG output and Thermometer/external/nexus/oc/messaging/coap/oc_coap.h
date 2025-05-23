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
//
// Modifications (c) 2020 Angaza, Inc.
*/

#ifndef OC_COAP_H
#define OC_COAP_H

// Added stdint as we are currently excluding 'separate.h'
#include <stdint.h>
//#include "separate.h"
#include "utils/oc_list.h"

#ifdef __cplusplus
extern "C"
{
#endif
struct oc_response_buffer_s
{
  uint8_t *buffer;
  uint16_t buffer_size;
  uint16_t response_length;
  int code;
};

#ifdef __cplusplus
}
#endif

#endif /* OC_COAP_H */
