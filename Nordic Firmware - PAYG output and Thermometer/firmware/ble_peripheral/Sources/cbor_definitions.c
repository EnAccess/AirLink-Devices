/* Functions for enccoding and decoding cbor messages
 * 
 * Created on 12/08/2021
 * Encoding and decoding of negative integers not yet Implemented 
 * Decoding of Array not yet implemented
 * Protection against buffer overflow of the source not yet Implemented
 * This can occur when there are some data dropped during reception
 */
 

#include <stdio.h>
#include <stdlib.h>
#include "cbor_definitions.h"
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "SEGGER_RTT.h"

/*
 * 000 10111 ----- positive interger from 0-23 just 1 byte
 * 001 10111 ----- negative interger from 0-23 just 1 byte
 * 
 * 000 11000 (24) ----- positive interger from 0-2^8  (1 + 1 bytes final)
 * 000 11001 (25) ----- positive interger from 0-2^16 (1 + 2 bytes final)
 * 000 11010 (26) ----- positive interger from 0-2^32 (1 + 4 bytes final)
 * 000 11011 (27) ----- positive interger from 0-2^64 (1 + 8 bytes final)
 *
 * 001 11000 (24) ----- negative interger from 0-2^8  (1 + 1 bytes final)
 * 001 11001 (25) ----- negative interger from 0-2^16 (1 + 2 bytes final)
 * 001 11010 (26) ----- negative interger from 0-2^32 (1 + 4 bytes final)
 * 001 11011 (27) ----- negative interger from 0-2^64 (1 + 8 bytes final)
 *
 */
 uint32_t byte_cbor8_encode(uint8_t * destination, uint32_t start_index, uint8_t value){
	 if(value<=23){
			destination[start_index]    = 0x00; 						// Insert positive interger header	
			destination[start_index++] |= value;						// Insert the number;
	 }
	 else{
			destination[start_index]    = 0x00;							// Insert positive interger header
			destination[start_index++] |= 0x18;							// Insert length as 1-byte interger (24)
			destination[start_index++]  = value;						// Insert the number;
	 }
	 return start_index;
 }

 uint32_t byte_cbor16_encode(uint8_t * destination, uint32_t start_index, uint16_t value){
	
	destination[start_index]   = 0x00;									// Insert positive interger header
	destination[start_index++]|= 0x19;									// Insert length as 2-byte interger (25)
	
	destination[start_index++] = (value>>8) & 0xff;			// Insert the MSB number;
	destination[start_index++] = value & 0xff;					// Insert the LSB number;
    return start_index;
 }

 uint32_t byte_cbor32_encode(uint8_t * destination, uint32_t start_index, uint32_t value){
    
    destination[start_index]   = 0x00;								// Insert positive interger header
	destination[start_index++]|= 0x1A;									// Insert length as 2-byte interger (25)
	
	destination[start_index++] = (value>>24) & 0xff;		// Insert the MSB number;
	destination[start_index++] = (value>>16) & 0xff;    // Insert the MSB number;
	destination[start_index++] = (value>>8) & 0xff;			// Insert the LSB number;
	destination[start_index++] = value & 0xff;					// Insert the LSB number;
	return start_index;
 }
 
 /*
  * 000 10111 ----- positive interger from 0-23 just 1 byte
  * 010 xxxxx ----- Type 2, is unstructured byte string, xxxxx is length string, 31 means indefinite length string
  * 011 xxxxx ----- Type 3, is UTF-8 structured byte string,  xxxxx is length string, 31 means indefinite length string
  */
 
 uint32_t byte_cborStr_encode(uint8_t * destination, uint32_t start_index, uint8_t * value, uint32_t length, bool structured){
	 //length--;                                                                              // Remove null character              
	 //printf("%u ",length);
	 if(structured)
	    destination[start_index]  = 0x60; 							// Insert UTF-8 header
	 else
	    destination[start_index]  = 0x40;
	 
	 if(length<=23){
		destination[start_index++] |= length;							// Insert the number;
	 }
	 else if((length<=255)){
		destination[start_index++]  |= 0x18;							// Insert length as 1-byte interger (24)
        destination[start_index++]   = length;	 
	 }
	 else if((length<=65535)){
		destination[start_index++]  |= 0x19;												// Insert length as 1-byte interger (24)
        destination[start_index++]   = (length>>8) & 0xff;      // Insert length msb                
        destination[start_index++]   = (length) & 0xff;         // Insert Length lsb
	 }
	 else{
		destination[start_index++]  |= 0x1A;								    		// Insert length as 1-byte interger (24)
        destination[start_index++] = (length>>24) & 0xff;				// Insert the MSB number;
	    destination[start_index++] = (length>>16) & 0xff;         // Insert the MSB number;
	    destination[start_index++] = (length>>8) & 0xff;					// Insert the LSB number;
	    destination[start_index++] = length & 0xff;								// Insert the LSB number;
	 }
	 for(uint32_t i=0; i<length; i++){
	    destination[start_index+i] = value[i];
	 }
	 return start_index+length;
 }

 
 /*
  * 000 10111 ----- positive interger from 0-23 just 1 byte
  * 100 xxxxx ----- Type 4, has a count field encoding the number of following items, followed by that many items
  * 101 xxxxx ----- Type 5, is similar but encodes a map (also called a dictionary, or associative array) of key/value pairs.
  */
 
 uint32_t byte_cborArr_encode(uint8_t * destination, uint32_t start_index, uint32_t length, bool is_array){
	 if(is_array)
		destination[start_index]  = 0x80; 												  // Insert array header	
	 else
		destination[start_index]  = 0xA0; 												  // Insert map header	
	 if(length<=23){
		destination[start_index++] |= length;											  // Insert the number;
	 }
	 else if((length<=255)){
		destination[start_index++]  |= 0x18;											  // Insert length as 1-byte interger (24)
        destination[start_index++]   = length;	 
	 }
	 else if((length<=65535)){
		destination[start_index++]  |= 0x19;											  // Insert length as 1-byte interger (25)
        destination[start_index++]   = (length>>8) & 0xff;      // Insert length msb                
        destination[start_index++]   = (length) & 0xff;         // Insert Length lsb
	 }
	 else{
		destination[start_index++]  |= 0x1A;											  // Insert length as 1-byte interger (26)
        destination[start_index++] = (length>>24) & 0xff;				// Insert the MSB number;
	    destination[start_index++] = (length>>16) & 0xff;         // Insert the MSB number;
	    destination[start_index++] = (length>>8) & 0xff;					// Insert the LSB number;
	    destination[start_index++] = length & 0xff;								// Insert the LSB number;
	 }
	 return start_index;
 }

 /*
 * 000 10111 ----- positive interger from 0-23 just 1 byte
 * 001 10111 ----- negative interger from 0-23 just 1 byte
 * 
 * 000 11000 (24) ----- positive interger from 0-2^8  (1 + 1 bytes final)
 * 000 11001 (25) ----- positive interger from 0-2^16 (1 + 2 bytes final)
 * 000 11010 (26) ----- positive interger from 0-2^32 (1 + 4 bytes final)
 * 000 11011 (27) ----- positive interger from 0-2^64 (1 + 8 bytes final)
 *
 * 001 11000 (24) ----- negative interger from 0-2^8  (1 + 1 bytes final)
 * 001 11001 (25) ----- negative interger from 0-2^16 (1 + 2 bytes final)
 * 001 11010 (26) ----- negative interger from 0-2^32 (1 + 4 bytes final)
 * 001 11011 (27) ----- negative interger from 0-2^64 (1 + 8 bytes final)
 *
 */

 
 /**************************************************** Decoding Starting here ****************************************************/
 
 uint8_t byte_cbor32_decode(uint8_t * source, void * destination, uint8_t start_index, uint8_t source_length, uint8_t destination_size){
    uint32_t result=0;
    uint8_t num_index=0;
    if((source[start_index]>>5)!=0)
        return 0;
    else if(source[start_index]<=23 && destination_size>=sizeof(uint8_t)){               // uint8_t <24
        result = source[start_index];
        //*(uint8_t *) destination = result;
        num_index = 1;
        //printf("value:%u\r\n",*(uint8_t *) destination);    
    }
    else if(source[start_index]==24 && destination_size>=sizeof(uint8_t)){               // uint8_t 
        result = source[start_index+1];
         //*(uint8_t *) destination = result;
         num_index = 2;
         //printf("value:%u\r\n",*(uint8_t *) destination);    
    }
    else if(source[start_index]==25 && destination_size>=sizeof(uint16_t)){               // uint16_t
        result |= (uint16_t)source[start_index+1]<<8;
        result |= source[start_index+2];
        //*(uint16_t *) destination = result;
        num_index = 3;
        //printf("value:%u\r\n",*(uint16_t *) destination);    
    }
    else if(source[start_index]==26 && destination_size>=sizeof(uint32_t)){               // uint32_t
        result |= (uint32_t)source[start_index+1]<<24;
        result |= (uint32_t)source[start_index+2]<<16;
        result |= (uint32_t)source[start_index+3]<<8;
        result |= source[start_index+4];
        //SEGGER_RTT_printf(0,"int32:%d:",result);
        num_index = 5;
    }
    else
        return 0;
		if(start_index+num_index>source_length)	// Prevents source overflow
			return 0;
    if(destination_size==sizeof(uint8_t)){
        *(uint8_t *) destination = result;
    }
    else if(destination_size==sizeof(uint16_t)){
        *(uint16_t *) destination = result;
    }
    else if(destination_size==sizeof(uint32_t)){
        *(uint32_t *) destination = result;
    }
    return start_index+num_index;
 }
 
 /*
  * 000 10111 ----- positive interger from 0-23 just 1 byte
  * 010 xxxxx ----- Type 2, is unstructured byte string, xxxxx is length string, 31 means indefinite length string
  * 011 xxxxx ----- Type 3, is UTF-8 structured byte string,  xxxxx is length string, 31 means indefinite length string
  */
 
 uint8_t byte_cborStr_decode(uint8_t * source, uint8_t * destination, uint8_t start_index, uint8_t source_length, uint8_t destination_length){
	//max_length--;                                                                              // Remove null character              
	//printf("%u ",length);
	uint8_t str_length=0;
    uint8_t num_index=0;
    
	if((source[start_index]>>5)!=2 && (source[start_index]>>5)!=3)
	    return 0;
	else if((source[start_index]&0X1F)<=23){               // uint8_t <24
			str_length = source[start_index]&0X1F;
			num_index = 1;
	}
	else if((source[start_index]&0X1F)==24){               // uint8_t 
			str_length = source[start_index+1];
			num_index = 2;
	}
	/* MTU LIMIT THE SIZE TO LESS THAN 247
	else if(source[start_index]<=25){               // uint16_t
			result |= (uint16_t)source[start_index+1]<<8;
			result |= source[start_index+2];
	}
	else if(source[start_index]<=26){               // uint32_t
			result |= (uint32_t)source[start_index+1]<<24;
			result |= (uint32_t)source[start_index+2]<<16;
			result |= (uint32_t)source[start_index+3]<<8;
			result |= source[start_index+4];
	}
	*/
	else{
			return 0;
	}
	
	if(start_index+num_index+str_length>source_length)	// Prevents source overflow
			return 0;
	//SEGGER_RTT_printf(0,"str_length:%d:",str_length);
	if(str_length>destination_length){				// Prevents Destination overflow
	    return 0;
	}
	else if(destination_length>str_length){		// Add termination Character at the end of string
		
	    destination[str_length] = 0x00;
	}
	 for(uint8_t i=0; i<str_length; i++){
	    sprintf((char *)(&destination[i]),"%c",source[start_index+i+num_index]);
	 }
	 return start_index+str_length+num_index;
 }


 /*
  * 000 10111 ----- positive interger from 0-23 just 1 byte
  * 100 xxxxx ----- Type 4, has a count field encoding the number of following items, followed by that many items
  * 101 xxxxx ----- Type 5, is similar but encodes a map (also called a dictionary, or associative array) of key/value pairs.
  */
 
 uint8_t byte_cborMap_decode_start(uint8_t * source, uint8_t start_index, uint8_t *length, bool *is_infinity){
	uint32_t map_length=0;
    uint8_t num_index=0;
    *is_infinity = false;
    
	if(source[start_index]>>5 != 5)
		return false; 												  // Insert array header	
	else if((source[start_index]&0X1F)<=23){               // uint8_t <24
        map_length = source[start_index]&0X1F;
        num_index = 1;
    }
    else if((source[start_index]&0X1F)==24){               // uint8_t 
        map_length = source[start_index+1];
        num_index = 2;
    }
    else if((source[start_index]&0X1F)==31){               // uint8_t 
        num_index = 1;
        *is_infinity = true;
        //printf("is_infinity:%s\r\n","true");
    }
    /* MTU LIMIT THE SIZE TO LESS THAN 247
    else if(source[start_index]<=25){               // uint16_t
        result |= (uint16_t)source[start_index+1]<<8;
        result |= source[start_index+2];
    }
    else if(source[start_index]<=26){               // uint32_t
        result |= (uint32_t)source[start_index+1]<<24;
        result |= (uint32_t)source[start_index+2]<<16;
        result |= (uint32_t)source[start_index+3]<<8;
        result |= source[start_index+4];
    }
    */
    else
        return false;
    *length = map_length;    
	return start_index+num_index;
 }
 // The values 20–23 are used to encode the special values false, true, null, and undefined. Values 0–19 are not currently defined.
 uint8_t byte_cborSpecial_decode(uint8_t * source, void * destination, uint8_t start_index){
     if(source[start_index]>>5 != 7)
        return false;
     else if((source[start_index]&0X1F)==20){
        *(bool *) destination = false;
        return start_index+1;
     }
     else if((source[start_index]&0X1F)==21){
        *(bool *) destination = true;
        return start_index+1;
     }
     else if((source[start_index]&0X1F)==22){
        *(bool *) destination = 0;                      // null
        return start_index+1;
     }
     else if((source[start_index]&0X1F)==23){
        *(uint8_t *) destination = 0xff;                // undefined
        return start_index+1;
     }
     else{
        return false;
     }
 }    

 bool byte_cborMap_decode_find(uint8_t * source, void * destination, uint8_t *find, uint8_t source_length){
	uint8_t map_length = 0;
	uint8_t temp[100];
	uint32_t int32_t_temp = 0;
	uint8_t temp_str[100]={0};
	bool map_infinity;
	uint8_t primitive_temp =0;
	
    uint32_t start_index = byte_cborMap_decode_start(source, 0, &map_length, &map_infinity);
    if(start_index == 0){
       return false; 
    }
    else if (map_infinity == true){
        map_length = 255;               // We dont expect map length of 255
    }
    bool value_found = false;
    for(uint32_t i=0;i<map_length;i++){
        // Read identifier 
        //printf("bf id:%d\r\n",start_index);
        if(map_infinity && source[start_index]==0xff){        // check the primitive at the end of String
            //printf("primitive found \r\n");
            if(value_found)
                return true;
            else
                return false;
        }
        start_index = byte_cborStr_decode(source, temp, start_index, source_length,sizeof(temp));
        //printf("bv:%d\r\n",start_index);
        if(start_index == 0 && map_infinity==false)
            return false;
        
        // pick value
        if(source[start_index]>>5 == 0){                                                // Positive interger found
            //int32_t_temp = 0;
            start_index = byte_cbor32_decode(source, &int32_t_temp, start_index, source_length,sizeof(int32_t_temp));
            //printf("%s:%u\r\n",temp,int32_t_temp);
						if(start_index && !strcmp((char *)temp,(char *)find)){
                *(uint32_t *)destination = int32_t_temp;
								//SEGGER_RTT_printf(0,"int decor:%d:",*(uint32_t *)destination);
                value_found = true;
                //return true;
            }
        }
        else if((source[start_index]>>5 == 2) || (source[start_index]>>5 == 3)) {       // String found
            uint32_t next_index = byte_cborStr_decode(source, temp_str, start_index, source_length, sizeof(temp_str));
            if(next_index && !strcmp((char *)temp,(char *)find)){
                /* Reconstructing string length Stage 1*/
								uint8_t str_length = 0;
								if((source[start_index]&0X1F)<=23){               // uint8_t <24
										str_length = source[start_index]&0X1F;
								}
								else if((source[start_index]&0X1F)==24){               // uint8_t 
										str_length = source[start_index+1];
								}
								//SEGGER_RTT_printf(0,"int str_length:%d:",str_length);
								//uint8_t str_length = next_index-start_index;			 									
								//printf("temp_str:\"%s\"\r\n",temp_str);
                //strcpy((char *)destination,(char *)temp_str);
                memcpy((uint8_t *)destination,temp_str,str_length+1);				// +1 is neccessary to make use the string is properly terminated
								value_found = true;
                //return true;
            }
						start_index = next_index;
        }
        else if((source[start_index]>>5 == 7)){
            //printf("l:%u\r\n",start_index);
            start_index = byte_cborSpecial_decode(source, &primitive_temp, start_index);
            //printf("l2:%d\r\n",primitive_temp);
            if(start_index && primitive_temp == true && !strcmp((char *)temp,(char *)find)){ 
                *(uint8_t *)destination = true;
                value_found = true;
                //return true;
            }
            else if(start_index && primitive_temp == false && !strcmp((char *)temp,(char *)find)){ 
                *(uint8_t *)destination = false;
                value_found = true;
                //return true;
            }    
            else if(map_infinity && primitive_temp == 0xff){
                // Will not reach here
                break;
            }
        }
        else{
            //printf("temp_str:%d\r\n",start_index);
            // Negative Interger not implemented
            // Array not implemented
            return false;
        }
        //printf("I:%d\r\n",i);
            
        // Check primitive
    }
    if(value_found)
        return true;
    else
        return false;
 }   
 
 
 //static void cbor_test(void){
	 /*
	 uint8_t p[1000];
   uint8_t index = 0;
   index = byte_cborArr_encode(p,index,6);
   index = byte_cbor8_encode(p,index,22);
   index = byte_cbor8_encode(p,index,221);
   index = byte_cbor16_encode(p,index,22123);
   index = byte_cbor32_encode(p,index,34000000);
   uint8_t data[] = "abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefg";
   index = byte_cborStr_encode(p,index,data,sizeof(data),true);
   index = byte_cbor8_encode(p,index,22);
   
   for (int i=0;i < index;i++) 
    printf("%02x ",p[i]);
	 */
   // decode test
	 /*
		uint8_t x[]={0x1a,0x00,0x00,0x00,0xff};
    uint32_t y=78;
    uint8_t z = byte_cbor32_decode(x, &y, 0, sizeof(y));
    
    uint8_t a[] = {0x78,0x4B,0x41,0x63,0x63,0x6F,0x75,0x6E,0x74,0x20,0x6F,0x77,0x6E,0x65,0x72,0x20,0x74,0x68, \
    0x61,0x74,0x20,0x70,0x65,0x72,0x66,0x6F,0x72,0x6D,0x73,0x20,0x74,0x61,0x73,0x6B,0x73,0x20,0x72,0x65,0x71, \
    0x75,0x69,0x72,0x69,0x6E,0x67,0x20,0x75,0x6E,0x72,0x65,0x73,0x74,0x72,0x69,0x63,0x74,0x65,0x64,0x20,0x61, \
    0x63,0x63,0x65,0x73,0x73,0x2E,0x20,0x4C,0x65,0x61,0x72,0x6E,0x20,0x6D,0x6F,0x72,0x65};
    uint8_t c[100]={0};
    uint8_t b = byte_cborStr_decode(a, c, 0, sizeof(c));
    
    printf("y:%u\r\n",y);
    printf("z:%d\r\n",z);

    printf("c:%s\r\n",c);
    printf("b:%d\r\n",b);
    
    uint8_t d[] = {0xA2,0x62,0x76,0x62,0x19,0x2D,0x73,0x62,0x63,0x70,0x62,0x68,0x69};
    
    uint8_t map_length = 0;
    bool map_infinity;
    bool e = byte_cborMap_decode_start(d, 0, &map_length, &map_infinity);
    
    printf("map_length:%d\r\n",map_length);
    printf("map_infinity:%d\r\n",map_infinity);
    printf("e:%d\r\n",e);
    
    uint16_t g=0;
    uint8_t h[20];
    bool f = byte_cborMap_decode_find(d, &g, "vb");
    bool j = byte_cborMap_decode_find(d, &h, "cp");
    printf("f:%d\r\n",f);
    printf("g:%u\r\n",g);
    printf("j:%d\r\n",j);
    printf("h:%s\r\n",h);
    */
    //A46372747219EA6163746B6E1A075BCD156274731A0DFB38D2626C741A0DFB38D3
    /*
		uint8_t data[] = {0xA4,0x63,0x72,0x74,0x72,0x19,0xEA,0x61,0x63,0x74,0x6B,0x6E,0x1A,0x07,0x5B,0xCD,0x15,
                      0x62,0x74,0x73,0x1A,0x0D,0xFB,0x38,0xD2,0x62,0x6C,0x74,0x1A,0x0D,0xFB,0x38,0xD3};
    uint8_t data2[] = {0xBF,0x62,0x76,0x62,0x19,0x2D,0x73,0x62,0x63,0x70,0x18,0x32,0x62,0x74,0x68,0x14,
                       0x62,0x63,0x61,0x19,0x7D,0x00,0x62,0x64,0x73,0xF5,0x62,0x63,0x67,0xF5,0x62,0x6C,
                       0x62,0xF5,0x62,0x66,0x74,0xF4,0x62,0x73,0x73,0x04,0x56,0xFF};
    
    uint16_t rtr = 0;
    uint32_t tkn = 0;
    uint32_t ts = 0;
    uint32_t lt = 0;
    */
    /*
    byte_cborMap_decode_find(data, &rtr, "rtr");
    byte_cborMap_decode_find(data, &tkn, "tkn");
    byte_cborMap_decode_find(data, &ts, "ts");
    byte_cborMap_decode_find(data, &lt, "lt");
    
    printf("rtr:%u\r\n",rtr);
    printf("tkn:%u\r\n",tkn);
    printf("ts:%u\r\n",ts);
    printf("lt:%u\r\n",lt);
    */
    
    //uint32_t t=98;
    /*
    byte_cborMap_decode_find(data2, &t, "vb");
    printf("vb:%u\r\n",t);
    byte_cborMap_decode_find(data2, &t, "cp");
    printf("cp:%u\r\n",t);
    byte_cborMap_decode_find(data2, &t, "th");
    printf("th:%u\r\n",t);
    byte_cborMap_decode_find(data2, &t, "ca");
    printf("ca:%u\r\n",t);
    byte_cborMap_decode_find(data2, &t, "ds");
    printf("ds:%d\r\n",(uint8_t)t);
    byte_cborMap_decode_find(data2, &t, "ft");
    printf("ft:%d\r\n",(uint8_t)t);
    */
    /*
		byte_cborMap_decode_find(data2, &t, "ss");
    printf("ss:%u\r\n",t);
    t=0;
    byte_cborMap_decode_find(data2, &t, "st");
    printf("st:%u\r\n",t);
    */
 //}
 
