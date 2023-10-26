#ifndef CBOR_DEFINITIONS_H_
#define CBOR_DEFINITIONS_H_
#include <stdint.h>
#include <stdbool.h>

//typedef struct ble_device_discovery_service_s ble_device_discovery_service_t;

uint32_t byte_cbor8_encode(uint8_t * destination, uint32_t start_index, uint8_t value);
uint32_t byte_cbor16_encode(uint8_t * destination, uint32_t start_index, uint16_t value);
uint32_t byte_cbor32_encode(uint8_t * destination, uint32_t start_index, uint32_t value);
uint32_t byte_cborStr_encode(uint8_t * destination, uint32_t start_index, uint8_t * value, uint32_t length, bool structured);
uint32_t byte_cborArr_encode(uint8_t * destination, uint32_t start_index, uint32_t length, bool is_array);
uint8_t byte_cbor32_decode(uint8_t * source, void * destination, uint8_t start_index, uint8_t source_length, uint8_t destination_size);
uint8_t byte_cborStr_decode(uint8_t * source, uint8_t * destination, uint8_t start_index, uint8_t source_length, uint8_t destination_length);
uint8_t byte_cborMap_decode_start(uint8_t * source, uint8_t start_index, uint8_t *length, bool *is_infinity);
uint8_t byte_cborSpecial_decode(uint8_t * source, void * destination, uint8_t start_index);
bool byte_cborMap_decode_find(uint8_t * source, void * destination, uint8_t *find, uint8_t source_length);
/*
typedef struct cbor_data_s
{
    uint8_t*                            data;
    uint8_t                             length;	
} cbor_data_t;
*/
#endif 
