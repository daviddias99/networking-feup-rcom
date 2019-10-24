#ifndef TLV_H
#define TLV_H


#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


typedef enum data_type {
    FILE_SIZE = 0,
    FILE_NAME
} data_type;

typedef enum value_type {
    STRING = 0,
    INT
} value_type;

typedef struct tlv_package {
    data_type type;
    uint8_t length;
    uint8_t* value;
    value_type value_type;
} tlv;


tlv* create_tlv_int(data_type type, int value);
tlv* create_tlv_str(data_type type, char* value);

void destroy_tlv(tlv* tlv_ptr);

void print_tlv(tlv* tlv_ptr);

#endif
