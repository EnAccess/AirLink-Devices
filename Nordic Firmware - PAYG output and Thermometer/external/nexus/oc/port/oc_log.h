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
/**
  @file
*/
#ifndef OC_LOG_H
#define OC_LOG_H

#include <stdio.h>

// Nexus user config to enable/disable logging helpers
#include "include/user_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

#ifdef CONFIG_NEXUS_COMMON_OC_PRINT_LOG_ENABLED

#define PRINT(...) printf(__VA_ARGS__)

#define PRINTipaddr(endpoint)                                                  \
  do {                                                                         \
      PRINT(                                                                   \
        "[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%"    \
        "02x%"                                                                 \
        "02x]:%d",                                                             \
        ((endpoint).addr.ipv6.address)[0], ((endpoint).addr.ipv6.address)[1],  \
        ((endpoint).addr.ipv6.address)[2], ((endpoint).addr.ipv6.address)[3],  \
        ((endpoint).addr.ipv6.address)[4], ((endpoint).addr.ipv6.address)[5],  \
        ((endpoint).addr.ipv6.address)[6], ((endpoint).addr.ipv6.address)[7],  \
        ((endpoint).addr.ipv6.address)[8], ((endpoint).addr.ipv6.address)[9],  \
        ((endpoint).addr.ipv6.address)[10],                                    \
        ((endpoint).addr.ipv6.address)[11],                                    \
        ((endpoint).addr.ipv6.address)[12],                                    \
        ((endpoint).addr.ipv6.address)[13],                                    \
        ((endpoint).addr.ipv6.address)[14],                                    \
        ((endpoint).addr.ipv6.address)[15], (endpoint).addr.ipv6.port);        \
  } while (0)

#define PRINTipaddr_local(endpoint)                                            \
  do {                                                                         \
    if ((endpoint).flags & IPV4) {                                             \
      PRINT("[%d.%d.%d.%d]:%d", ((endpoint).addr_local.ipv4.address)[0],       \
            ((endpoint).addr_local.ipv4.address)[1],                           \
            ((endpoint).addr_local.ipv4.address)[2],                           \
            ((endpoint).addr_local.ipv4.address)[3],                           \
            (endpoint).addr_local.ipv4.port);                                  \
    } else {                                                                   \
      PRINT(                                                                   \
        "[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%"    \
        "02x%"                                                                 \
        "02x]:%d",                                                             \
        ((endpoint).addr_local.ipv6.address)[0],                               \
        ((endpoint).addr_local.ipv6.address)[1],                               \
        ((endpoint).addr_local.ipv6.address)[2],                               \
        ((endpoint).addr_local.ipv6.address)[3],                               \
        ((endpoint).addr_local.ipv6.address)[4],                               \
        ((endpoint).addr_local.ipv6.address)[5],                               \
        ((endpoint).addr_local.ipv6.address)[6],                               \
        ((endpoint).addr_local.ipv6.address)[7],                               \
        ((endpoint).addr_local.ipv6.address)[8],                               \
        ((endpoint).addr_local.ipv6.address)[9],                               \
        ((endpoint).addr_local.ipv6.address)[10],                              \
        ((endpoint).addr_local.ipv6.address)[11],                              \
        ((endpoint).addr_local.ipv6.address)[12],                              \
        ((endpoint).addr_local.ipv6.address)[13],                              \
        ((endpoint).addr_local.ipv6.address)[14],                              \
        ((endpoint).addr_local.ipv6.address)[15],                              \
        (endpoint).addr_local.ipv6.port);                                      \
    }                                                                          \
  } while (0)
#define PRINTbytes(bytes, length)                                              \
  do {                                                                         \
    uint16_t za_unshadowed_i;                                                  \
    for (za_unshadowed_i = 0; za_unshadowed_i < length; za_unshadowed_i++)     \
      PRINT(" %02X", bytes[za_unshadowed_i]);                                  \
    PRINT("\n");                                                               \
  } while (0)
#else
#define PRINT(...)
#define PRINTipaddr(endpoint)
#define PRINTipaddr_local(endpoint)
#define PRINTbytes(bytes, length)
#endif /* CONFIG_NEXUS_COMMON_OC_PRINT_LOG_ENABLED */

#if defined(OC_DEBUG) || defined(CONFIG_NEXUS_COMMON_OC_DEBUG_LOG_ENABLED)
#define OC_LOG(level, ...)                                                     \
  do {                                                                         \
    PRINT("%s: %s <%s:%d>: ", level, __FILE__, __func__, __LINE__);            \
    PRINT(__VA_ARGS__);                                                        \
    PRINT("\n");                                                               \
  } while (0)
#define OC_DBG(...) OC_LOG("DEBUG", __VA_ARGS__)
#define OC_WRN(...) OC_LOG("WARNING", __VA_ARGS__)
#define OC_ERR(...) OC_LOG("ERROR", __VA_ARGS__)
#define OC_LOGipaddr(endpoint)                                                 \
  do {                                                                         \
    PRINT("DEBUG: %s <%s:%d>: ", __FILE__, __func__, __LINE__);                \
    PRINTipaddr(endpoint);                                                     \
    PRINT("\n");                                                               \
  } while (0)
#define OC_LOGbytes(bytes, length)                                             \
  do {                                                                         \
    PRINT("DEBUG: %s <%s:%d>: ", __FILE__, __func__, __LINE__);                \
    uint16_t i;                                                                \
    for (i = 0; i < length; i++)                                               \
      PRINT(" %02X", bytes[i]);                                                \
    PRINT("\n");                                                               \
  } while (0)
#else
#define OC_LOG(...)
#define OC_DBG(...)
#define OC_WRN(...)
#define OC_ERR(...)
#define OC_LOGbytes(bytes, length)
#define OC_LOGipaddr(endpoint)
#endif /* defined(OC_DEBUG) || defined(CONFIG_NEXUS_COMMON_OC_DEBUG_LOG_ENABLED) */

#ifdef __cplusplus
}
#endif

#endif /* OC_LOG_H */
