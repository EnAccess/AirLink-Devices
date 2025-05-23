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


//#include "messaging/coap/engine.h"
#include "messaging/coap/oc_coap.h"
//#include "messaging/coap/separate.h"

#include "oc_api.h"

/*
#ifdef OC_SECURITY
#include "security/oc_store.h"
#endif // OC_SECURITY

#if defined(OC_COLLECTIONS) && defined(OC_SERVER)
#include "oc_collection.h"
#endif // OC_COLLECTIONS && OC_SERVER

#ifdef OC_DYNAMIC_ALLOCATION
#include <stdlib.h>
#endif // OC_DYNAMIC_ALLOCATION
*/
#include "oc_core_res.h"
//static size_t query_iterator;

int
oc_add_device(const char *uri, const char *rt, const char *name,
              const char *spec_version, const char *data_model_version,
              oc_add_device_cb_t add_device_cb, void *data)
{
  if (!oc_core_add_new_device(uri, rt, name, spec_version, data_model_version,
                              add_device_cb, data))
    return -1;
  return 0;
}

int
oc_init_platform(const char *mfg_name, oc_init_platform_cb_t init_platform_cb,
                 void *data)
{
  if (!oc_core_init_platform(mfg_name, init_platform_cb, data))
    return -1;
  return 0;
}
/*
int
oc_get_query_value(oc_request_t *request, const char *key, char **value)
{
  if (!request)
    return -1;
  return oc_ri_get_query_value(request->query, request->query_len, key, value);
}

*/
static int
response_length(void)
{
  int size = oc_rep_get_encoded_payload_size();
  return (size <= 2) ? 0 : size;
}

void
oc_send_response(oc_request_t *request, oc_status_t response_code)
{
  request->response->response_buffer->response_length =
    (uint16_t)response_length();
  request->response->response_buffer->code = oc_status_code(response_code);
}
/*
void
oc_ignore_request(oc_request_t *request)
{
  request->response->response_buffer->code = OC_IGNORE;
}

void
oc_set_immutable_device_identifier(size_t device, oc_uuid_t *piid)
{
  if (piid && device < oc_core_get_num_devices()) {
    oc_device_info_t *info = oc_core_get_device_info(device);
    if (info) {
#ifdef OC_SECURITY
      oc_sec_load_unique_ids(device);
#endif // OC_SECURITY
      memcpy(info->piid.id, piid->id, sizeof(oc_uuid_t));
#ifdef OC_SECURITY
      oc_sec_dump_unique_ids(device);
#endif // OC_SECURITY
    }
  }
}
*/

void
oc_set_delayed_callback(void *cb_data, oc_trigger_t callback, uint16_t seconds)
{
  oc_ri_add_timed_event_callback_seconds(cb_data, callback, seconds);
}

void
oc_remove_delayed_callback(void *cb_data, oc_trigger_t callback)
{
  oc_ri_remove_timed_event_callback(cb_data, callback);
}

void
oc_process_baseline_interface(oc_resource_t *resource)
{
  if (oc_string_len(resource->name) > 0) {
    oc_rep_set_text_string(root, n, oc_string(resource->name));
  }
  oc_rep_set_string_array(root, rt, resource->types);
  oc_core_encode_interfaces_mask(oc_rep_object(root), resource->interfaces);
}
/*
void
oc_init_query_iterator(void)
{
  query_iterator = 0;
}

int
oc_iterate_query(oc_request_t *request, char **key, size_t *key_len,
                 char **value, size_t *value_len)
{
  query_iterator++;
  return oc_ri_get_query_nth_key_value(request->query, request->query_len, key,
                                       key_len, value, value_len,
                                       query_iterator);
}

bool
oc_iterate_query_get_values(oc_request_t *request, const char *key,
                            char **value, int *value_len)
{
  char *current_key = 0;
  size_t key_len = 0, v_len;
  int pos = 0;

  do {
    pos = oc_iterate_query(request, &current_key, &key_len, value, &v_len);
    *value_len = (int)v_len;
    if (pos != -1 && strlen(key) == key_len &&
        memcmp(key, current_key, key_len) == 0) {
      goto more_or_done;
    }
  } while (pos != -1);

  *value_len = -1;

more_or_done:
  if (pos == -1 || (size_t)pos >= request->query_len) {
    return false;
  }
  return true;
}

#ifdef OC_SERVER
*/

static void
oc_populate_resource_object(oc_resource_t *resource, const char *name,
                            const char *uri, uint8_t num_resource_types,
                            size_t device)
{
  if (name) {
    oc_new_string(&resource->name, name, strlen(name));
  } else {
    memset(&resource->name, 0, sizeof(oc_string_t));
  }
  oc_store_uri(uri, &resource->uri);
  oc_new_string_array(&resource->types, num_resource_types);
  resource->properties = (oc_resource_properties_t) 0;
  resource->device = device;

/*
#ifdef OC_SECURITY
  resource->properties |= OC_SECURE;
#endif // OC_SECURITY
*/
}

oc_resource_t *
oc_new_resource(const char *name, const char *uri, uint8_t num_resource_types,
                size_t device)
{
  oc_resource_t *resource = oc_ri_alloc_resource();
  if (resource) {
    resource->interfaces = OC_IF_BASELINE;
    resource->default_interface = OC_IF_BASELINE;
    resource->observe_period_seconds = 0;
    resource->num_observers = 0;
    oc_populate_resource_object(resource, name, uri, num_resource_types,
                                device);
  }
  return resource;
}

/*
#if defined(OC_COLLECTIONS)
oc_resource_t *
oc_new_collection(const char *name, const char *uri, uint8_t num_resource_types,
                  size_t device)
{
  oc_collection_t *collection = oc_collection_alloc();
  if (collection) {
    collection->interfaces = OC_IF_BASELINE | OC_IF_LL | OC_IF_B;
    collection->default_interface = OC_IF_LL;
    oc_populate_resource_object((oc_resource_t *)collection, name, uri,
                                num_resource_types, device);
  }
  return (oc_resource_t *)collection;
}

void
oc_delete_collection(oc_resource_t *collection)
{
  oc_collection_free((oc_collection_t *)collection);
}

void
oc_add_collection(oc_resource_t *collection)
{
  oc_resource_set_observable(collection, true);
  oc_collection_add((oc_collection_t *)collection);
}

oc_resource_t *
oc_collection_get_collections(void)
{
  return (oc_resource_t *)oc_collection_get_all();
}
#endif // OC_COLLECTIONS
*/

void
oc_resource_bind_resource_interface(oc_resource_t *resource,
                                    oc_interface_mask_t iface_mask)
{
  resource->interfaces = (oc_interface_mask_t) (resource->interfaces | iface_mask);
}

void
oc_resource_set_default_interface(oc_resource_t *resource,
                                  oc_interface_mask_t iface_mask)
{
  resource->default_interface = iface_mask;
}

void
oc_resource_bind_resource_type(oc_resource_t *resource, const char *type)
{
  oc_string_array_add_item(resource->types, (char *)type);
}

/*
#ifdef OC_SECURITY
void
oc_resource_make_public(oc_resource_t *resource)
{
  resource->properties &= ~OC_SECURE;
}
#endif // OC_SECURITY

void
oc_resource_set_discoverable(oc_resource_t *resource, bool state)
{
  if (state)
    resource->properties |= OC_DISCOVERABLE;
  else
    resource->properties &= ~OC_DISCOVERABLE;
}

void
oc_resource_set_observable(oc_resource_t *resource, bool state)
{
  if (state)
    resource->properties |= OC_OBSERVABLE;
  else
    resource->properties &= ~(OC_OBSERVABLE | OC_PERIODIC);
}

void
oc_resource_set_periodic_observable(oc_resource_t *resource, uint16_t seconds)
{
  resource->properties |= OC_OBSERVABLE | OC_PERIODIC;
  resource->observe_period_seconds = seconds;
}

void
oc_resource_set_properties_cbs(oc_resource_t *resource,
                               oc_get_properties_cb_t get_properties,
                               void *get_props_user_data,
                               oc_set_properties_cb_t set_properties,
                               void *set_props_user_data)
{
  resource->get_properties.cb.get_props = get_properties;
  resource->get_properties.user_data = get_props_user_data;
  resource->set_properties.cb.set_props = set_properties;
  resource->set_properties.user_data = set_props_user_data;
}
*/
void
oc_resource_set_request_handler(oc_resource_t *resource, oc_method_t method,
                                oc_request_callback_t callback, void *user_data)
{
  oc_request_handler_t *handler = NULL;
  switch (method) {
  case OC_GET:
    handler = &resource->get_handler;
    break;
  case OC_POST:
    handler = &resource->post_handler;
    break;
  case OC_PUT:
    handler = &resource->put_handler;
    break;
  case OC_DELETE:
    handler = &resource->delete_handler;
    break;
  default:
    break;
  }

  handler->cb = callback;
  handler->user_data = user_data;
}

/*
void
oc_set_con_write_cb(oc_con_write_cb_t callback)
{
  size_t i;
  for (i = 0; i < oc_core_get_num_devices(); i++) {
    oc_resource_t *res = oc_core_get_resource_by_index(OCF_CON, i);
    if (res) {
      res->post_handler.user_data = *(void **)(&callback);
    }
  }
}
*/

bool
oc_add_resource(oc_resource_t *resource)
{
  return oc_ri_add_resource(resource);
}


