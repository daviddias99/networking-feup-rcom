#ifndef APP_UTIL_H
#define APP_UTIL_H


#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdint.h"

typedef enum packet_type {
    DATA = 1,
    START,
    END
} packet_type;

void log_control_packet(uint8_t* packet, uint8_t packet_size);
void log_data_packet(uint8_t* packet);
void progress_bar(const char* prefix, size_t count, size_t max_count);

#endif