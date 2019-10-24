#ifndef APP_H
#define APP_H


#include "string.h"
#include "stdlib.h"
#include "stdio.h"


#define MAX_FILE_SIZE_MB            1
#define MB_TO_B(x)                  ((size_t) (x) << 20)   
#define MAX_FILE_SIZE_B             MB_TO_B(MAX_FILE_SIZE_MB)



typedef struct file_info {
    char* path;
    int size;
} file_info;

typedef enum packet_type {
    DATA = 1,
    START,
    END
} packet_type;


void log_control_packet(uint8_t* packet, uint8_t packet_size);
void log_data_packet(uint8_t* packet);



#endif